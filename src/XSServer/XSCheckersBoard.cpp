#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSServer/XSServerGame.h"
#include "XSServer/XSCheckersBoard.h"
#include "XSNetwork/XSNetwork.h"

namespace XS {

	namespace ServerGame {

		CheckersBoard::CheckersBoard() {
			// generate the pieces
			for ( size_t i = 0u; i < numPieces; i++ ) {
				CheckersPiece *piece = &pieces[i];

				// assign them a colour
				if ( i < numPieces / 2u ) {
					piece->colour = CheckersPiece::Colour::Red;
				}
				else {
					piece->colour = CheckersPiece::Colour::Black;
				}

				// calculate their offset in the virtual board
				const uint8_t piecesPerRow = 4u;
				uint8_t row = i / piecesPerRow;
				if ( row > 2u ) {
					row += 2u;
				}
				uint8_t col = (i % piecesPerRow) * 2u + (row % 2u == 0u);
				piece->offset = (row * dimensions) + col;
			}
		}

		void CheckersBoard::Update( real64_t dt ) {
			//TODO: check for kings
			for ( auto &piece : pieces ) {
				if ( piece.colour == CheckersPiece::Colour::Red ) {
					const uint8_t row = piece.offset / dimensions;
					if ( row == dimensions - 1 ) {
						piece.king = true;
					}
				}
				else {
					const uint8_t row = piece.offset / dimensions;
					if ( row == 0u ) {
						piece.king = true;
					}
				}
			}
			//TODO: check end-game rules
		}

		static CheckersPiece::Colour GetOppositeColour( CheckersPiece::Colour colour ) {
			if ( colour == CheckersPiece::Colour::Red ) {
				return CheckersPiece::Colour::Black;
			}
			else {
				return CheckersPiece::Colour::Red;
			}
		}

		void CheckersBoard::UpdatePiece( uint8_t offsetFrom, uint8_t offsetTo ) {
			CheckersPiece::Colour nextMove;
			for ( auto &piece : pieces ) {
				if ( piece.offset == offsetFrom ) {
					if ( piece.colour != state.currentMove ) {
						// tried to move a piece not belonging to the current player
						return;
					}
					// check for capturing
					const uint8_t fromCol = offsetFrom % dimensions;
					const uint8_t toCol = offsetTo % dimensions;

					if ( std::abs( fromCol - toCol ) == 2 ) {
						// jumped
						const uint8_t jumpOffset = (offsetFrom + offsetTo) / 2;
						for ( auto &jumpPiece : pieces ) {
							if ( jumpPiece.offset == jumpOffset ) {
								//FIXME: just swap to end of list and track how many active pieces there are
								jumpPiece.offset = 0xFFu;
								jumpPiece.valid = false;
								break;
							}
						}

						nextMove = piece.colour;
					}
					else {
						nextMove = GetOppositeColour( piece.colour );
					}
					piece.offset = offsetTo;

					Network::XSPacket outPacket( Network::ID_XS_SV2CL_MOVE_PIECE );
					ByteBuffer bbOut;
					bbOut.WriteInt8( offsetFrom );
					bbOut.WriteInt8( offsetTo );
					outPacket.data = bbOut.GetMemory( &outPacket.dataLen );

					console.Print( PrintLevel::Debug, "Send ID_XS_SV2CL_MOVE_PIECE: %i to %i\n",
						offsetFrom,
						offsetTo
					);

					Network::Send( 0u, &outPacket );

					break;
				}
			}

			SetCurrentPlayer( nextMove );
		}

	} // namespace ServerGame

} // namespace XS
