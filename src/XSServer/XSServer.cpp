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
			delete serverConsole;
		}

		static void GenerateNetworkState( void ) {
#if 0
			Network::XSPacket snapshot( Network::ID_XS_SV2CL_GAMESTATE );

			ByteBuffer buffer;
			ServerGame::GenerateSnapshot( &buffer );
			snapshot.data = buffer.GetMemory( &snapshot.dataLen );

			for ( auto *client : clients ) {
				Network::Send( client->guid, &snapshot );
			}
#endif
		}

		void NetworkPump( void ) {
			if ( !Network::IsActive() ) {
				return;
			}

			// handle generic messages
			// this will make a call to Server::ReceivePacket( Packet *p )
			Network::Receive();

			//TODO: process player input

			// issue a snapshot
			//FIXME: stop sending the entire state
			GenerateNetworkState();
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

				Network::XSPacket outPacket( Network::ID_XS_SV2CL_MOVE_PIECE );
				ByteBuffer bbOut;
				bbOut.WriteInt8( offsetFrom );
				bbOut.WriteInt8( offsetTo );
				outPacket.data = bbOut.GetMemory( &outPacket.dataLen );
				Network::Send( 0u, &outPacket );
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

			static real64_t lastMsgTime = 0.0;
			real64_t currentTime = GetElapsedTime();
			if ( currentTime - lastMsgTime >= 2500 ) {
				lastMsgTime = currentTime;
				BroadcastMessage( String::Format( "Automated message at %.5f\n", currentTime ).c_str() );
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
