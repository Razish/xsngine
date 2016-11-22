#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <unordered_map>
#include <map>
#include <algorithm>

#include <RakNet/RakPeerInterface.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakString.h>

#include "Common/Common.h"
#include "Common/Console.h"
#include "Common/Globals.h"
#include "Common/Cvar.h"
#include "Common/Command.h"
#include "Common/ByteBuffer.h"
#include "Client/Client.h"
#include "Network/Network.h"
#include "Server/Server.h"

namespace Network {

	//FIXME: wrap up in a class
	static RakNet::RakPeerInterface *peer = nullptr;
	static bool connected = false;
	static uint32_t maxConnections = 1u;
	static bool	isServer = false;
	static GUID myGUID = 0u;

	static std::unordered_map<GUID, Connection *> connections;

	static Cvar *net_debug = nullptr;
	static Cvar *net_port = nullptr;

	void XSPacket::Send( GUID guid ) const {
		SendPacket( guid, *this );
	}

	Connection::Connection( GUID guid )
	: state( privateState ), guid( guid )
	{
		// ...
	}
	Connection::~Connection() {
		// ...
	}

	// note: recursive for now
	Connection &Connection::Get( GUID guid ) {
		auto it = connections.find( guid );
		if ( it != connections.end() ) {
			return *it->second;
		}

		// doesn't already exist, allocate a new one
		connections[guid] = new Connection( guid );
		return *connections[guid];
	}

	using StateList = std::vector<Connection::State>;
	using GraphContainer = std::map<Connection::State, StateList>;
	static const GraphContainer connectionTransitionGraph {
		{
			Connection::State::NotConnected, {
				Connection::State::SynSent
			}
		},
		{
			Connection::State::SynSent, {
				Connection::State::SynReceived
			}
		},
		{
			Connection::State::SynReceived, {
				Connection::State::AckSent
			}
		},
		{
			Connection::State::AckSent, {
				Connection::State::AckReceived
			}
		},
		{
			Connection::State::AckReceived, {
				Connection::State::Active
			}
		},
		{
			Connection::State::Active, {
				Connection::State::Disconnecting,
				Connection::State::Dead
			}
		},
		{
			Connection::State::Disconnecting, {
				Connection::State::Dead
			}
		},
	};

	static const std::map<Connection::State, const char *> stateToString {
		{ Connection::State::NotConnected,	"NotConnected" },
		{ Connection::State::SynSent,		"SynSent" },
		{ Connection::State::SynReceived,	"SynReceived" },
		{ Connection::State::AckSent,		"AckSent" },
		{ Connection::State::AckReceived,	"AckReceived" },
		{ Connection::State::Active,		"Active" },
		{ Connection::State::Disconnecting,	"Disconnecting" },
		{ Connection::State::Dead,			"Dead" }
	};

	const char *Connection::StateToString( Connection::State state ) {
		const auto &it = stateToString.find( state );
		if ( it == stateToString.end() ) {
			return "Unknown";
		}
		else {
			return it->second;
		}
	}

	bool Connection::ChangeState( Connection::State newState, bool doNetwork ) {
		if ( state == newState ) {
			// nothing to do
			return false;
		}

		const GraphContainer::const_iterator &it = connectionTransitionGraph.find( privateState );
		const StateList &node = it->second;
		if ( node.empty() || std::find( node.begin(), node.end(), newState ) == node.end() ) {
			console.Print( PrintLevel::Developer,
				"connection %" PRIX64 " state change failed: %s -> %s not allowed\n",
				guid,
				Connection::StateToString( state ),
				Connection::StateToString( newState )
			);
			return false;
		}

		console.Print( PrintLevel::Developer,
			"connection %" PRIX64 " state change (%s -> %s)\n",
			guid,
			Connection::StateToString( privateState ),
			Connection::StateToString( newState )
		);

		if ( doNetwork ) {
			ByteBuffer stateBuffer;
			if ( stateBuffer.Write<State>( newState ) != ByteBuffer::Error::Success ) {
				// ...
			}

			XSPacket statePacket( isServer ? ID_XS_SV2CL_CONNECTION_STATE : ID_XS_CL2SV_CONNECTION_STATE );
			statePacket.data = stateBuffer.GetMemory( &statePacket.dataLen );
			statePacket.Send( guid );
		}

		privateState = newState;
		return true;
	}

	void Connection::Send( const XSPacket &packet ) const {
		packet.Send( guid );
	}

	static void Cmd_NetStat( const CommandContext &context ) {
		PrintStatus();
	}

	static void RegisterCommands( void ) {
		Command::AddCommand( "netstat", Cmd_NetStat );
	}

	static void RegisterCvars( void ) {
		net_debug = Cvar::Create( "net_debug", "0",
			"Print debug messages of network activity", CVAR_ARCHIVE
		);
		net_port = Cvar::Create( "net_port", "1337",
			"Network port to listen on", CVAR_ARCHIVE
		);
	}

	void Init( void ) {
		RegisterCommands();
		RegisterCvars();

		isServer = Common::com_dedicated->GetBool();
		peer = RakNet::RakPeerInterface::GetInstance();
		uint16_t port = static_cast<uint16_t>( net_port->GetInt32() );

		RakNet::SocketDescriptor socketDescriptors[] = {
			RakNet::SocketDescriptor( port, nullptr ) // primary network card
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
		for ( auto &it : connections ) {
			Connection *connection = it.second;
			delete connection;
			it.second = nullptr;
		}
		Disconnect();

		if ( peer ) {
			peer->Shutdown( 500, 0, PacketPriority::LOW_PRIORITY );
			RakNet::RakPeerInterface::DestroyInstance( peer );
			peer = nullptr;
		}
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

	//FIXME: move all of this into Connection
	bool IsConnected( void ) {
		return IsActive() && connected;
	}

	bool IsActive( void ) {
		return peer != nullptr;
	}

	bool Connect( const char *hostname, uint16_t port ) {
		if ( !hostname || !hostname[0] ) {
			hostname = "127.0.0.1";
		}

		if ( !port ) {
			port = net_port->GetInt32();
		}

		if ( isServer ) {
			console.Print( PrintLevel::Normal, "Can't connect to a host as the server\n" );

			return false;
		}

		console.Print( PrintLevel::Normal, "Connecting to %s:%hd...\n",
			hostname,
			port
		);

		RakNet::ConnectionAttemptResult result = peer->Connect( hostname, port, nullptr, 0 );
		if ( result != RakNet::CONNECTION_ATTEMPT_STARTED ) {
			console.Print( PrintLevel::Normal, "Failed to connect: %s\n",
				connectionAttemptResultMessages[result]
			);
			return false;
		}

		return true;
	}

	void Disconnect( void ) {
		if ( !IsConnected() ) {
			return;
		}

		if ( isServer ) {
			//FIXME: when does this happen?
		}
		else {
			console.Print( PrintLevel::Normal, "closing network connection.\n" );
			peer->CloseConnection( peer->GetSystemAddressFromIndex( 0 ), true );
		}

		connected = false;
	}

	void Receive( void ) {
		SDL_assert( peer );

		RakNet::Packet *packet = nullptr;
		while ( (packet = peer->Receive()) ) {
			if ( net_debug->GetUInt32() & 0x2u ) {
				console.Print( PrintLevel::Normal, "Receive: %i (%i)\n",
					packet->data[0],
					ID_USER_PACKET_ENUM
				);
			}

			switch ( packet->data[0] ) {

			case ID_REMOTE_DISCONNECTION_NOTIFICATION: {
				// another client has disconnected gracefully
				console.Print( PrintLevel::Normal, "another client disconnected\n" );
				//TODO: grab meaningful info
			} break;

			case ID_CONNECTION_ATTEMPT_FAILED: {
				// connection attempt failed
				console.Print( PrintLevel::Normal, "error: connection attempt failed (%s)\n",
					packet->systemAddress.ToString()
				);
			} break;

			case ID_REMOTE_CONNECTION_LOST: {
				// another client has lost connection
				console.Print( PrintLevel::Normal, "another client lost connection\n" );
				//TODO: grab meaningful info
			} break;

			case ID_NEW_INCOMING_CONNECTION: {
				// another client has connected
				if ( isServer ) {
					const GUID guid = packet->guid.g;
					console.Print( PrintLevel::Normal, "client connecting with IP %s and GUID %" PRIX64 "\n",
						packet->systemAddress.ToString(),
						guid
					);
					Server::IncomingConnection( Connection::Get( guid ) );
				}
				else {
					SDL_assert( !"unexpected connection as client" );
				}
			} break;

			case ID_REMOTE_NEW_INCOMING_CONNECTION: {
				// another client has connected
				console.Print( PrintLevel::Normal, "client connecting with IP %s and GUID %s\n",
					packet->systemAddress.ToString(),
					packet->guid.ToString()
				);
			} break;

			case ID_ALREADY_CONNECTED: {
				// a client with our GUID is already connected, wait for timeout because we probably disconnected
				//	ungracefully
				//TODO: filter for server/client?
				console.Print( PrintLevel::Normal, "error: already connected, wait for timeout\n" );
				connected = false;
			} break;

			case ID_CONNECTION_REQUEST_ACCEPTED: {
				// out connection request has been accepted
				console.Print( PrintLevel::Developer, "connection accepted\n" );
				connected = true;
				if ( !isServer ) {
					const GUID guid = packet->guid.g;
					Client::Connect( Connection::Get( guid ) );
				}
			} break;

			case ID_NO_FREE_INCOMING_CONNECTIONS: {
				// the server is full
				console.Print( PrintLevel::Normal, "error: server is full\n" );
				connected = false;
			} break;

			case ID_DISCONNECTION_NOTIFICATION: {
				const GUID guid = packet->guid.g;
				// we have been disconnected
				if ( isServer ) {
					console.Print( PrintLevel::Normal, "client disconnected (%s)\n",
						packet->systemAddress.ToString()
					);
					Server::DropClient( guid );
					connections.erase( guid );
				}
				else {
					console.Print( PrintLevel::Normal, "disconnected from server (%s)\n",
						packet->systemAddress.ToString()
					);
					Client::Disconnect( guid );
					Disconnect();
				}
			} break;

			case ID_CONNECTION_LOST: {
				// our connection was lost
				console.Print( PrintLevel::Normal, "error: connection lost\n" );
				connected = false;
			} break;

			default: {
				if ( isServer ) {
					if ( Server::ReceivePacket( packet ) ) {
						return;
					}
				}
				else {
					if ( Client::ReceivePacket( packet ) ) {
						return;
					}
				}

				console.Print( PrintLevel::Developer, "Unknown message from %s (ID: %i, base: %i)\n",
					packet->systemAddress.ToString(),
					packet->data[0],
					ID_USER_PACKET_ENUM
				);
			} break;

			}

			peer->DeallocatePacket( packet );
		}
	}

	void SendPacket( GUID guid, const XSPacket &packet ) {
		if ( net_debug->GetUInt32() & 0x1u ) {
			console.Print( PrintLevel::Normal, "SendPacket: %i (%i)\n",
				packet.msg,
				ID_USER_PACKET_ENUM
			);
		}

		if ( !isServer ) {
			SDL_assert( connected );
		}

		RakNet::BitStream bs;

		// write message ID
		bs.Write(
			static_cast<RakNet::MessageID>( packet.msg )
		);

		// write message contents
		bs.Write(
			static_cast<const char *>( packet.data ),
			static_cast<unsigned int>( packet.dataLen )
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
			PacketPriority::IMMEDIATE_PRIORITY, // priority
			PacketReliability::RELIABLE_ORDERED, // reliability
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
			console.Print( PrintLevel::Normal, "Network primed on:\n" );
			for ( size_t i = 0u; i < numAddresses; i++ ) {
				Indent indent( 1u );
				console.Print( PrintLevel::Normal, "%i. %s\n",
					i + 1,
					peer->GetLocalIP( i )
				);
			}
		}
		else {
			console.Print( PrintLevel::Normal, "Network primed on %s\n",
				peer->GetMyBoundAddress().ToString()
			);
		}
		console.Print( PrintLevel::Normal, "GUID: %" PRIX64 "\n",
			myGUID
		);

		// get a list of remote connections
		uint16_t numRemoteSystems;
		peer->GetConnectionList( nullptr, &numRemoteSystems );
		RakNet::SystemAddress *remoteSystems = new RakNet::SystemAddress[numRemoteSystems];
		peer->GetConnectionList( remoteSystems, &numRemoteSystems );

		console.Print( PrintLevel::Normal, "%i connections (max: %i)\n",
			numRemoteSystems,
			peer->GetMaximumIncomingConnections()
		);
		if ( numRemoteSystems ) {
			console.Print( PrintLevel::Normal, "Listing active connections...\n" );
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

				console.Print( PrintLevel::Normal, "%s: %s - %s\n",
					type.c_str(),
					sa->ToString(),
					connectionStateMessages[peer->GetConnectionState( *sa )]
				);
			}
		}
		delete[] remoteSystems;
	}

} // namespace Network
