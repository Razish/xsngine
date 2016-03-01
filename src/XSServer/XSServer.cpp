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

			for ( auto &client : clients ) {
				client.second->connection.Send( &snapshotPacket );
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

			// issue a snapshot
			//FIXME: stop sending the entire state
			GenerateNetworkState();
		}

		bool ReceivePacket( const RakNet::Packet *packet ) {
			switch ( packet->data[0] ) {

			case Network::ID_XS_CL2SV_DUMMY: {
				// ...
			} break;

			default: {
				return false;
			} break;

			}

			return true;
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
