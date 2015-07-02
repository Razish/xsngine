#include <RakNet/RakPeerInterface.h>

#include <cstdio>

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
#include "XSNetwork/XSNetwork.h"
#include "XSSystem/XSOS.h"

namespace XS {

	namespace Server {

		Cvar *sv_maxConnections = nullptr;
		std::vector<Client *> clients;

		uint64_t frameNum = 0u;

		ServerConsole *serverConsole = nullptr;

		static void RegisterCvars( void ) {
			sv_maxConnections = Cvar::Create(
				"sv_maxConnections",
				"2",
				"Maximum number of client connections",
				CVAR_ARCHIVE
			);
		}

		void Init( void ) {
			RegisterCvars();

			Network::Init();

			serverConsole = new ServerConsole();

			ServerGame::Init();
		}

		void Shutdown( void ) {
			ServerGame::Shutdown();

			delete serverConsole;
		}

		void NetworkPump( void ) {
			if ( !Network::IsActive() ) {
				return;
			}

			// handle generic messages
			// this will make a call to Server::ReceivePacket( Packet *p )
			Network::Receive();
		}

		bool ReceivePacket( const RakNet::Packet *packet ) {

			switch ( packet->data[0] ) {

			case Network::ID_XS_CL2SV_MOVE_PIECE: {
				uint8_t *buffer = packet->data + 1;
				size_t bufferLen = packet->length - 1;
				ByteBuffer bbIn( buffer, bufferLen );

				uint8_t offsetFrom, offsetTo;
				bbIn.ReadUInt8( &offsetFrom );
				bbIn.ReadUInt8( &offsetTo );

				console.Print( PrintLevel::Debug, "Receive ID_XS_CL2SV_MOVE_PIECE: %i to %i\n",
					offsetFrom,
					offsetTo
				);

				ServerGame::UpdatePiece( offsetFrom, offsetTo );
			} break;

			default: {
				return false;
			} break;

			}

			return true;
		}

		void IncomingConnection( const RakNet::Packet *packet ) {
			Client *client = new Client( packet->guid.g );

			client->state = Client::State::Connecting;

			Network::XSPacket spPacket( Network::ID_XS_SV2CL_SET_PLAYER );
			ByteBuffer buffer;
			const char *team = clients.size() == 0 ? "Red" : "Black";
			buffer.WriteString( team );
			spPacket.data = buffer.GetMemory( &spPacket.dataLen );
			Network::Send( client->guid, &spPacket );

			console.Print( PrintLevel::Debug, "Send ID_XS_SV2CL_SET_PLAYER: %s\n",
				team
			);

			BroadcastMessage(
				String::Format(
					"Connection from %s, playing as %s\n",
					packet->systemAddress.ToString(),
					team
				).c_str()
			);

			clients.push_back( client );
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
			bb.WriteString( msg );
			packet.data = bb.GetMemory( &packet.dataLen );

			console.Print( PrintLevel::Debug, "Send ID_XS_SV2CL_PRINT: %s\n",
				msg
			);

			Network::Send( 0u, &packet );
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
