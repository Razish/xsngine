#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSNetwork/XSNetwork.h"
#include "XSServer/XSCheckersBoard.h"
#include "XSServer/XSServerGame.h"
#include "XSServer/XSServer.h"

namespace XS {

	namespace ServerGame {

		GameState state = {};
		CheckersBoard *board = nullptr;

		void Init( void ) {
			board = new CheckersBoard();
		}

		void RunFrame( real64_t dt ) {
			board->Update( dt );
		}

		void UpdatePiece( uint8_t offsetFrom, uint8_t offsetTo ) {
			board->UpdatePiece( offsetFrom, offsetTo );
		}

		void SetCurrentPlayer( CheckersPiece::Colour colour ) {
			state.currentMove = colour;
			Network::XSPacket outPacket( Network::ID_XS_SV2CL_SET_CURRENT_PLAYER );
			ByteBuffer bbOut;
			bbOut.WriteInt8( static_cast<uint8_t>( colour ) );
			outPacket.data = bbOut.GetMemory( &outPacket.dataLen );

			console.Print( PrintLevel::Debug, "Send ID_XS_SV2CL_SET_CURRENT_PLAYER: %s\n",
				(colour == CheckersPiece::Colour::Black)
					? "Black"
					: "Red"
			);

			Network::Send( 0u, &outPacket );
		}

		void Shutdown( void ) {
			delete board;
		}

	} // namespace ServerGame

} // namespace XS
