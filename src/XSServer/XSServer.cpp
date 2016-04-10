#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <algorithm>

#include <RakNet/RakPeerInterface.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSTimer.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSServer/XSServer.h"
#include "XSServer/XSServerGame.h"
#include "XSServer/XSServerConsole.h"
#include "XSServer/XSClient.h"
#include "XSServer/XSNetcode.h"
#include "XSServer/XSResources.h"
#include "XSNetwork/XSNetwork.h"
#include "XSSystem/XSOS.h"

namespace XS {

	namespace Server {

		Cvar *sv_maxConnections = nullptr;

		uint64_t frameNum = 0u;

		ServerConsole *serverConsole = nullptr;

		static void Cmd_CreateAccount( Client &client, const CommandContext &context ) {
			if ( client.account ) {
				client.Print(
					String::Format( "You are already logged into account %i\n", client.account->id ).c_str()
				);
				return;
			}

			if ( context.size() != 2 ) {
				client.Print( "usage: acct_create name password\n" );
			}

			const char *name = context[0].c_str();
			const char *password = context[1].c_str();
			Account *account = Account::Create( name, password );
			client.Print( String::Format( "Created account (id: %i)\n", account->id ).c_str() );
		}

		static void Cmd_AccountInfo( Client &client, const CommandContext &context ) {
			if ( !client.account ) {
				client.Print( "You are not logged into an account\n" );
				return;
			}

			// basic account info
			client.Print(
				String::Format( "Account %i:\n", client.account->id ).c_str()
			);
			client.Print(
				String::Format( "  name: %s\n", client.account->name.c_str() ).c_str()
			);

			// characters
			std::vector<Character *> characterList;
			Character::List( client.account->id, characterList );
			const size_t numCharacters = characterList.size();
			client.Print(
				String::Format( "  numCharacters: %" PRIuMAX "\n", numCharacters ).c_str()
			);
			for ( const Character *character : characterList ) {
				client.Print(
					String::Format( "  Character %i/%" PRIuMAX ":\n", character->id, numCharacters ).c_str()
				);
				client.Print(
					String::Format( "    name: %s\n", character->name.c_str() ).c_str()
				);

				// quests
				client.Print(
					String::Format( "    numQuests: %i\n", character->quests.activeQuests.size() ).c_str()
				);
				for ( QuestInstance::ID questID : character->quests.activeQuests ) {
					//const QuestInstance *quest = QuestInstance::GetFromID( questID );
					client.Print(
						String::Format( "      quest %i:\n", questID ).c_str()
					);
				}
			}
		}

		static void Cmd_Login( Client &client, const CommandContext &context ) {
			if ( client.account ) {
				client.Print(
					String::Format( "Already logged into account %i\n", client.account->id ).c_str()
				);
				return;
			}

			if ( context.size() != 2 ) {
				client.Print( "usage: acct_login name password\n" );
				return;
			}

			const char *name = context[0].c_str();
			const char *password = context[1].c_str();
			Account::LoginError loginStatus = client.Login( name, password );
			if ( loginStatus != Account::LoginError::Success ) {
				client.Print( String::Format( "Failed login: %i\n", loginStatus ).c_str() );
				return;
			}
			client.Print(
				String::Format( "Logged into account id: %i\n",
					client.account->id
				).c_str()
			);
		}

		static void Cmd_Logout( Client &client, const CommandContext &context ) {
			client.Logout();
		}

		static void Cmd_CreateCharacter( Client &client, const CommandContext &context ) {
			if ( !client.account ) {
				client.Print( "You are not logged into an account\n" );
				return;
			}

			if ( context.size() != 1 ) {
				client.Print( "usage: char_create name\n" );
				return;
			}

			const char *name = context[0].c_str();
			const Character *character = Character::Create( client.account->id, name );
			if ( !character ) {
				client.Print(
					String::Format( "Character \"%s\" already exists on account %i (character id: %i)\n",
						name, character->accountID, character->id
					).c_str()
				);
				return;
			}

			client.Print( String::Format( "Created character: %s\n", character->name.c_str() ).c_str() );
		}

		static void Cmd_SelectCharacter( Client &client, const CommandContext &context ) {
			if ( !client.account ) {
				client.Print( "You are not logged into an account\n" );
				return;
			}

			if ( context.size() != 1 ) {
				client.Print( "usage: char_select name\n" );
				return;
			}
			const char *name = context[0].c_str();
			Character *character = Character::Find( client.account->id, name );
			if ( !character ) {
				client.Print(
					String::Format( "Could not find character named %s for account %i\n",
						name,
						client.account->id
					).c_str()
				);
				return;
			}
			client.SelectCharacter( character );
		}

		static void Cmd_Ping( Client &client, const CommandContext &context ) {
			client.Print( "Pong!\n" );
		}

		static void Cmd_Say( Client &client, const CommandContext &context ) {
			ByteBuffer chatBuffer;
			// identifier of the sender
			if ( chatBuffer.Write<Network::GUID>( client.connection.guid ) != ByteBuffer::Error::Success ) {
				return;
			}
			// reserved for message flags
			if ( chatBuffer.Write<uint32_t>( 0u ) != ByteBuffer::Error::Success ) {
				return;
			}

			std::string msg;
			String::Join( context, " ", msg );
			String::Strip( msg, "\r\n" );
			// chat message
			if ( !msg.empty() && chatBuffer.WriteString( msg.c_str() ) != ByteBuffer::Error::Success ) {
				return;
			}
			Network::XSPacket chatPacket( Network::ID_XS_SV2CL_CHAT );
			chatPacket.data = chatBuffer.GetMemory( &chatPacket.dataLen );

			for ( auto &it : clients ) {
				Client *cl = it.second;
				if ( cl ) {
					cl->connection.Send( chatPacket );
				}
			}
		}

		using CommandContext = std::vector<std::string>;
		using CommandFunc = void (*)( Client &client, const CommandContext &context );
		static std::unordered_map<std::string, CommandFunc> clientCommands{
			{ "acct_create", Cmd_CreateAccount },
			{ "acct_info", Cmd_AccountInfo },
			{ "acct_login", Cmd_Login },
			{ "acct_logout", Cmd_Logout },
			{ "char_create", Cmd_CreateCharacter },
			{ "char_select", Cmd_SelectCharacter },
			{ "ping", Cmd_Ping },
			{ "say", Cmd_Say },
		};

		static void OnClientCommand( Client &client, const char *cmd, const CommandContext &context ) {
			auto it = clientCommands.find( cmd );
			if ( it == clientCommands.end() ) {
				// unknown command
				return;
			}
			it->second( client, context );
		}

		void DropClient( Network::GUID guid ) {
			Client *client = clients[guid];
			if ( client ) {
				console.Print( PrintLevel::Normal, "Dropping client with guid %" PRIX64 "\n",
					guid
				);

				delete client;
			}

			// announce
			BroadcastMessage( String::Format( "Client %" PRIX64 " has disconnected\n", guid ).c_str() );
		}

		void IncomingConnection( Network::Connection &connection ) {
			// drop client if they're already connected
			DropClient( connection.guid );

			BroadcastMessage( String::Format( "Connection from %" PRIX64, connection.guid ).c_str() );
		}

		bool ReceivePacket( const RakNet::Packet *packet ) {
			switch ( packet->data[0] ) {

			case Network::ID_XS_CL2SV_CONNECTION_STATE: {
				using State = Network::Connection::State;
				const Network::GUID guid = packet->guid.g;
				Network::Connection &connection = Network::Connection::Get( guid );

				const uint8_t *buffer = packet->data + 1;
				size_t bufferLen = packet->length - 1;
				ByteBuffer bb( buffer, bufferLen );

				State newState;
				if ( bb.Read<State>( &newState ) != ByteBuffer::Error::Success ) {
					// invalid packet format
					break;
				}

				if ( connection.ChangeState( newState ) ) {
					// the client is telling us their state has changed
					switch ( newState ) {

					case State::NotConnected: {
						// initial state: no connections requested
						// error: should never change to this
						SDL_assert( !"unhandled case" );
					} break;

					case State::SynSent: {
						// we request a connection to a host

						//TODO: send a challenge

						connection.ChangeState( State::SynReceived, true );
					} break;

					case State::SynReceived: {
						// the host sends us back a challenge
						SDL_assert( !"unhandled case" );
					} break;

					case State::AckSent: {
						// we respond to the challenge

						//TODO: check if challenge response is good

						//TODO: allocate client instance
						new Client( connection );

						//TODO: start sending session info
						ServerGame::NetworkResources( true );

						connection.ChangeState( State::AckReceived, true );
					} break;

					case State::AckReceived: {
						// host allocates client instance and starts sending session info
						SDL_assert( !"unhandled case" );
					} break;

					case State::Active: {
						// baselines have been receieved, connection is active
						// nothing to do at the moment
					} break;

					case State::Disconnecting: {
						// disconnection has been requested
						// this is handled by another RakNet message currently
						SDL_assert( !"unhandled case" );
					} break;

					case State::Dead: {
						// this host has finished disconnecting or has timed out
						SDL_assert( !"unhandled case" );
					} break;

					default: {
						SDL_assert( !"unhandled case" );
					} break;

					}
				}
			} break;

			case Network::ID_XS_CL2SV_COMMAND: {
				const Network::GUID guid = packet->guid.g;
				const Network::Connection &connection = Network::Connection::Get( guid );

				if ( connection.state != Network::Connection::State::Active ) {
					console.Print( PrintLevel::Developer,
						"client cmd from GUID %" PRIX64 " ignored during connection state %s\n",
						guid, connection.StateToString( connection.state )
					);
					break;
				}

				const uint8_t *buffer = packet->data + 1;
				size_t bufferLen = packet->length - 1;
				ByteBuffer bb( buffer, bufferLen );
				ByteBuffer::Error status;

				uint32_t numArgs = 0u;
				CommandContext context;
				if ( (status = bb.Read<uint32_t>( &numArgs )) == ByteBuffer::Error::Success ) {
					context.reserve( numArgs ); // pre-allocate the memory required for the number of arguments
				}

				ByteBuffer::String cmd;
				status = bb.ReadString( cmd );

				console.Print( PrintLevel::Debug, "client cmd from GUID %" PRIX64 ": %s\n",
					guid, cmd.c_str()
				);

				if ( numArgs > 0u ) {
					for ( uint32_t i = 0u; i < numArgs; i++ ) {
						ByteBuffer::String arg;
						status = bb.ReadString( arg );
						context.push_back( arg );
					}
				}

				for ( const auto &arg : context ) {
					console.Print( PrintLevel::Normal, "  arg: %s, len: %i\n",
						arg.c_str(), arg.length()
					);
				}

				OnClientCommand( *clients[guid], cmd.c_str(), context );
			} break;

			case Network::ID_XS_CL2SV_DUMMY: {
				// ...
			} break;

			default: {
				return false;
			} break;

			}

			return true;
		}

		static void RegisterCvars( void ) {
			sv_maxConnections = Cvar::Create( "sv_maxConnections", "16",
				"Maximum number of client connections", CVAR_ARCHIVE
			);
		}

		static void RegisterCommands( void ) {
			// ...
		}

		void Init( void ) {
			RegisterCvars();
			RegisterCommands();

			Network::Init();

			serverConsole = new ServerConsole();

			ServerGame::Init();
		}

		void Shutdown( void ) {
			ServerGame::Shutdown();

			delete serverConsole;

			Network::Shutdown();
		}

		static void GenerateNetworkState( void ) {
			// make sure resource list is in sync
			ServerGame::NetworkResources( false );

			// send snapshot of gamestate
			Network::XSPacket snapshotPacket( Network::ID_XS_SV2CL_GAMESTATE );

			ByteBuffer snapshotBuffer;
			ServerGame::GenerateSnapshot( &snapshotBuffer );
			snapshotPacket.data = snapshotBuffer.GetMemory( &snapshotPacket.dataLen );

			for ( auto &it : clients ) {
				const Client *client = it.second;
				if ( client ) {
					client->connection.Send( snapshotPacket );
				}
			}
		}

		void NetworkPump( void ) {
			if ( !Network::IsActive() ) {
				return;
			}

			// handle generic messages
			// this will make a call to Server::ReceivePacket( RakNet::Packet *packet )
			Network::Receive();

			//TODO: process player input
			ServerGame::NetworkResources( false );

			// issue a snapshot
			//FIXME: stop sending the entire state
			GenerateNetworkState();
		}

		void RunFrame( real64_t dt ) {
			static real64_t stepTime = 0.0;
			frameNum++;

			// previousState = currentState;
			// integrate( currentState, stepTime, dt );
			stepTime += dt;

			// simulate local entities
			//	this does not include clients, their inputs are processed as soon as they are received
			ServerGame::RunFrame( dt );

			if ( serverConsole ) {
				if ( const char *p = serverConsole->Read() ) {
					Command::Append( p );
				}
			}
		}

		void BroadcastMessage( const char *msg ) {
			Network::XSPacket packet( Network::ID_XS_SV2CL_PRINT );

			ByteBuffer bb;
			if ( bb.WriteString( msg ) != ByteBuffer::Error::Success ) {
				return;
			}
			packet.data = bb.GetMemory( &packet.dataLen );

			packet.Send( 0u );
		}

		// lazy initialise on first request per frame
		real64_t GetElapsedTime( TimerResolution resolution ) {
			static uint64_t lastFrame = 0u;
			static real64_t timeSec = 0.0;
			static real64_t timeMsec = 0.0;
			static real64_t timeUsec = 0.0;
			if ( lastFrame != frameNum ) {
				lastFrame = frameNum;
				timeUsec = Common::gameTimer->GetTiming();
				timeMsec = timeUsec * 0.001;
				timeSec = timeUsec * 0.000001;
			}

			switch ( resolution ) {

				case TimerResolution::Seconds: {
					return timeSec;
				} break;

				case TimerResolution::Milliseconds: {
					return timeMsec;
				} break;

				case TimerResolution::Microseconds: {
					return timeUsec;
				} break;

				default: {
					return 0.0;
				} break;
			}
		}

	} // namespace Server

} // namespace XS
