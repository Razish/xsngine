#include <RakNet/RakPeerInterface.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakString.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSNetwork/XSNetwork.h"

namespace XS {

	namespace Network {

		//FIXME: naughty "using namespace RakNet"
		using namespace RakNet;

		static RakPeerInterface *peer = nullptr;
		static bool connected = false;

		void Init( void ) {
			peer = RakPeerInterface::GetInstance();

			SocketDescriptor socketDescriptors[] = {
				SocketDescriptor() // primary network card
			};
			peer->Startup( 1, socketDescriptors, ARRAY_LEN( socketDescriptors ) );
			peer->SetMaximumIncomingConnections( 1 );

			console.Print( PrintLevel::Normal,
				"Network primed on %s\n",
				peer->GetMyBoundAddress().ToString()
			);
		}

		static const char *connectionAttemptResultMessages[] = {
			"Connection attempt started", // CONNECTION_ATTEMPT_STARTED
			"Invalid Parameter", // INVALID_PARAMETER
			"Cannot resolve domain name", // CANNOT_RESOLVE_DOMAIN_NAME
			"Already connected", // ALREADY_CONNECTED_TO_ENDPOINT
			"Connection attempt already in progress", // CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS
			"Security initialisation failed", // SECURITY_INITIALIZATION_FAILED
		};

		bool IsConnected( void ) {
			return connected;
		}

		bool Connect( const char *hostname, uint16_t port ) {
			console.Print( PrintLevel::Normal,
				"Connecting to %s:%hd...\n",
				hostname, port
			);

			ConnectionAttemptResult result = peer->Connect( hostname, port, nullptr, 0 );
			if ( result != CONNECTION_ATTEMPT_STARTED ) {
				console.Print( PrintLevel::Normal,
					"Failed to connect: %s\n", connectionAttemptResultMessages[result]
				);
				return false;
			}

			return true;
		}

		void Receive( void ) {
			SDL_assert( connected );

			Packet *packet = nullptr;
			while ( (packet = peer->Receive()) ) {

				switch( packet->data[0] ) {

				case ID_REMOTE_DISCONNECTION_NOTIFICATION: {
					// another client has disconnected gracefully
				} break;

				case ID_REMOTE_CONNECTION_LOST: {
					// another client has lost connection
				} break;

				case ID_REMOTE_NEW_INCOMING_CONNECTION: {
					// another client has connected
				} break;

				case ID_CONNECTION_REQUEST_ACCEPTED: {
					// out connection request has been accepted
					connected = true;
				} break;

				case ID_NO_FREE_INCOMING_CONNECTIONS: {
					// the server is full
				} break;

				case ID_DISCONNECTION_NOTIFICATION: {
					// we have been disconnected - server shutdown
					connected = false;
				} break;

				case ID_CONNECTION_LOST: {
					// our connection was lost
					connected = false;
				} break;

				case ID_XS_PING: {
					// ingame ping
					console.Print( PrintLevel::Normal,
						"Ping received\n"
					);
					SDL_assert( packet->length == 0u );
				} break;

				case ID_XS_TEXT_MESSAGE: {
					// text message
					BitStream bs( packet->data, packet->length, false );
					bs.IgnoreBytes( sizeof(MessageID) );

					RakString str;
					bs.Read( str );

					console.Print( PrintLevel::Normal,
						"message: %s\n", str.C_String()
					);
				} break;

				default: {
				} break;

				}

				peer->DeallocatePacket( packet );
			}
		}

		void Send( const XSPacket *packet ) {
			SDL_assert( connected );

			BitStream bs;
			bs.Write( static_cast<MessageID>( packet->msg ) );
			bs.Write( static_cast<const char *>( packet->data ), static_cast<unsigned int>( packet->dataLen ) );
			peer->Send(
				&bs,
				PacketPriority::MEDIUM_PRIORITY, // priority
				PacketReliability::RELIABLE_ORDERED, // reliability
				0, // orderingChannel
				UNASSIGNED_SYSTEM_ADDRESS, // systemIdentifier
				true // broadcast
			);
		}

	} // namespace Network

} // namespace XS
