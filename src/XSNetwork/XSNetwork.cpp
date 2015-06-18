#include <RakNet/RakPeerInterface.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakString.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSCommand.h"
#include "XSClient/XSClient.h"
#include "XSNetwork/XSNetwork.h"
#include "XSServer/XSServer.h"

namespace XS {

	namespace Network {

		//FIXME: wrap up in a class
		static RakNet::RakPeerInterface *peer = nullptr;
		static bool connected = false;
		static uint32_t maxConnections = 1u;
		static bool	isServer = false;
		static uint64_t myGUID = 0u;

		static Cvar *net_port = nullptr;

		static void Cmd_NetStat( const CommandContext * const context ) {
			PrintStatus();
		}

		static void RegisterCommands( void ) {
			Command::AddCommand( "netstat", Cmd_NetStat );
		}

		static void RegisterCvars( void ) {
			net_port = Cvar::Create( "net_port", "1337", "Network port to listen on", CVAR_ARCHIVE );
		}

		void Init( void ) {
			RegisterCommands();
			RegisterCvars();

			isServer = Common::com_dedicated->GetBool();
			peer = RakNet::RakPeerInterface::GetInstance();
			uint16_t port = static_cast<uint16_t>( net_port->GetInt32() );

			RakNet::SocketDescriptor socketDescriptors[] = {
				RakNet::SocketDescriptor( port + isServer, nullptr ) // primary network card
			};

			if ( isServer ) {
				maxConnections = Server::sv_maxConnections->GetUInt32();
			}
			else {
				maxConnections = 1u;
			}
			peer->Startup( maxConnections, socketDescriptors, ARRAY_LEN( socketDescriptors ) );
			peer->SetMaximumIncomingConnections( maxConnections );
		//	peer->SetOccasionalPing( true );
			peer->SetUnreliableTimeout( 1000 );
			peer->AllowConnectionResponseIPMigration( false );

			myGUID = peer->GetGuidFromSystemAddress( RakNet::UNASSIGNED_SYSTEM_ADDRESS ).g;
			PrintStatus();
		}

		void Shutdown( void ) {
			connected = false;

			//FIXME: peer->CloseConnection( ... )?
			peer->Shutdown( 500, 0, PacketPriority::LOW_PRIORITY );
			RakNet::RakPeerInterface::DestroyInstance( peer );
			peer = nullptr;
		}

		static const char *connectionAttemptResultMessages[] = {
			"Connection attempt started", // CONNECTION_ATTEMPT_STARTED
			"Invalid Parameter", // INVALID_PARAMETER
			"Cannot resolve domain name", // CANNOT_RESOLVE_DOMAIN_NAME
			"Already connected", // ALREADY_CONNECTED_TO_ENDPOINT
			"Connection attempt already in progress", // CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS
			"Security initialisation failed", // SECURITY_INITIALIZATION_FAILED
		};

		static const char *connectionStateMessages[] = {
			"pending", // IS_PENDING, connect() was called, but the process hasn't started yet
			"connecting", // IS_CONNECTING, processing the connection attempt
			"connected", // IS_CONNECTED, is connected and able to communicate
			"disconnecting", // IS_DISCONNECTING, was connected, but will disconnect as soon as the remaining messages are delivered
			"aborting", // IS_SILENTLY_DISCONNECTING, a connection attempt failed and will be aborted
			"no longer connected", // IS_DISCONNECTED, no longer connected
			"not connected", // IS_NOT_CONNECTED, was never connected, or else was disconnected long enough ago that the entry has been discarded
		};

		bool IsConnected( void ) {
			return connected;
		}

		bool IsActive( void ) {
			return peer != nullptr;
		}

		bool Connect( const char *hostname, uint16_t port ) {
			if ( !hostname || !hostname[0] ) {
				hostname = "127.0.0.1";
			}

			if ( !port ) {
				port = net_port->GetInt32() + !isServer;
			}

			if ( isServer ) {
				console.Print( PrintLevel::Normal,
					"Can't connect to a host as the server\n"
				);

				return false;
			}

			console.Print( PrintLevel::Normal,
				"Connecting to %s:%hd...\n",
				hostname, port
			);

			RakNet::ConnectionAttemptResult result = peer->Connect( hostname, port, nullptr, 0 );
			if ( result != RakNet::CONNECTION_ATTEMPT_STARTED ) {
				console.Print( PrintLevel::Normal,
					"Failed to connect: %s\n",
					connectionAttemptResultMessages[result]
				);
				return false;
			}

			return true;
		}

		void Disconnect( void ) {
			if ( connected ) {
				console.Print( PrintLevel::Normal,
					"Not connected to a server\n"
				);
				return;
			}

			if ( isServer ) {
			}
			else {
				console.Print( PrintLevel::Normal,
					"Closing network connection.\n"
				);
				peer->CloseConnection( peer->GetSystemAddressFromIndex( 0 ), true );
			}

			connected = false;
		}

		void Receive( void ) {
			SDL_assert( peer );

			RakNet::Packet *packet = nullptr;
			while ( (packet = peer->Receive()) ) {
#if 0
				console.Print( PrintLevel::Debug,
					"Receive: %i (%i)\n",
					packet->data[0],
					ID_USER_PACKET_ENUM
				);
#endif
				switch ( packet->data[0] ) {

				case ID_REMOTE_DISCONNECTION_NOTIFICATION: {
					// another client has disconnected gracefully
					console.Print( PrintLevel::Normal,
						"another client disconnected\n"
					);
				} break;

				case ID_CONNECTION_ATTEMPT_FAILED: {
					// connection attempt failed
					console.Print( PrintLevel::Normal,
						"connection attempt failed (%s)\n",
						packet->systemAddress.ToString()
					);
				} break;

				case ID_REMOTE_CONNECTION_LOST: {
					// another client has lost connection
					console.Print( PrintLevel::Normal,
						"another client lost connection\n"
					);
				} break;

				case ID_NEW_INCOMING_CONNECTION: {
					// another client has connected
					if ( Common::com_dedicated->GetBool() ) {
						console.Print( PrintLevel::Normal,
							"client connecting with IP %s and GUID %s\n",
							packet->systemAddress.ToString(),
							packet->guid.ToString()
						);
						Server::IncomingConnection( packet );
					}
					else {
						SDL_assert( !"unexpected connection as client" );
					}
				} break;

				case ID_REMOTE_NEW_INCOMING_CONNECTION: {
					// another client has connected
					console.Print( PrintLevel::Normal,
						"client connecting with IP %s and GUID %s\n",
						packet->systemAddress.ToString(),
						packet->guid.ToString()
					);
				} break;

				case ID_CONNECTION_REQUEST_ACCEPTED: {
					// out connection request has been accepted
					console.Print( PrintLevel::Normal,
						"connection accepted\n"
					);
					connected = true;
				} break;

				case ID_NO_FREE_INCOMING_CONNECTIONS: {
					// the server is full
					console.Print( PrintLevel::Normal,
						"server is full\n"
					);
					connected = false;
				} break;

				case ID_DISCONNECTION_NOTIFICATION: {
					// we have been disconnected - server shutdown
					console.Print( PrintLevel::Normal,
						"we have been disconnected - server shutdown\n"
					);
					connected = false;
				} break;

				case ID_CONNECTION_LOST: {
					// our connection was lost
					console.Print( PrintLevel::Normal,
						"connection lost\n"
					);
					connected = false;
				} break;

				default: {
					if ( Common::com_dedicated->GetBool() ) {
						if ( Server::ReceivePacket( packet ) ) {
							return;
						}
					}
					else {
						if ( Client::ReceivePacket( packet ) ) {
							return;
						}
					}

					console.Print( PrintLevel::Developer,
						"Unknown message from %s (ID: %i, base: %i)\n",
						packet->systemAddress.ToString(),
						packet->data[0],
						ID_USER_PACKET_ENUM
					);
				} break;

				}

				peer->DeallocatePacket( packet );
			}
		}

		void Send( uint64_t guid, const XSPacket *packet ) {
#if 0
			console.Print( PrintLevel::Debug,
				"Send: %i (%i)\n",
				packet->msg,
				ID_USER_PACKET_ENUM
			);
#endif

			if ( !isServer ) {
				SDL_assert( connected );
			}

			RakNet::BitStream bs;

			// write message ID
			bs.Write(
				static_cast<RakNet::MessageID>( packet->msg )
			);

			// write message contents
			bs.Write(
				static_cast<const char *>( packet->data ),
				static_cast<unsigned int>( packet->dataLen )
			);

			// send it to the correct peer
			RakNet::SystemAddress systemIdentifier;
			if ( isServer ) {
				if ( guid == 0u ) {
					systemIdentifier = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
				}
				else {
					systemIdentifier = peer->GetSystemAddressFromGuid( RakNet::RakNetGUID( guid ) );
				}
			}
			else {
				//FIXME: retrieve from connection packet?
				systemIdentifier = peer->GetSystemAddressFromIndex( 0 );
			}

			peer->Send(
				&bs, // bitStream
				PacketPriority::MEDIUM_PRIORITY, // priority
				PacketReliability::RELIABLE, // reliability
				0, // orderingChannel
				systemIdentifier, // systemIdentifier
				isServer // broadcast
					? guid == 0u
					: false
			);
		}

		void PrintStatus( void ) {
			// enumerate local addresses
			size_t numAddresses = peer->GetNumberOfAddresses();
			if ( numAddresses ) {
				console.Print( PrintLevel::Normal,
					"Network primed on:\n"
				);
				for ( size_t i = 0u; i < numAddresses; i++ ) {
					Indent indent( 1u );
					console.Print( PrintLevel::Normal,
						"%i. %s\n",
						i + 1,
						peer->GetLocalIP( i )
					);
				}
			}
			else {
				console.Print( PrintLevel::Normal,
					"Network primed on %s\n",
					peer->GetMyBoundAddress().ToString()
				);
			}
			console.Print( PrintLevel::Normal,
				"GUID: %X\n",
				myGUID
			);

			// get a list of remote connections
			uint16_t numRemoteSystems;
			peer->GetConnectionList( nullptr, &numRemoteSystems );
			RakNet::SystemAddress *remoteSystems = new RakNet::SystemAddress[numRemoteSystems];
			peer->GetConnectionList( remoteSystems, &numRemoteSystems );

			console.Print( PrintLevel::Normal,
				"%i connections (max: %i)\n",
				numRemoteSystems,
				peer->GetMaximumIncomingConnections()
			);
			if ( numRemoteSystems ) {
				console.Print( PrintLevel::Normal,
					"Listing active connections...\n"
				);
				for ( size_t i = 0u; i < numRemoteSystems; i++ ) {
					RakNet::SystemAddress *sa = &remoteSystems[i];
					Indent indent( 1u );

					std::string type;
					if ( sa->IsLANAddress() ) {
						type = " LAN";
					}
					else if ( sa->IsLoopback() ) {
						type = "LOOP";
					}
					else {
						type = "INET";
					}

					console.Print( PrintLevel::Normal,
						"%s: %s - %s\n",
						type.c_str(),
						sa->ToString(),
						connectionStateMessages[peer->GetConnectionState( *sa )]
					);
				}
			}
			delete remoteSystems;
		}

	} // namespace Network

} // namespace XS
