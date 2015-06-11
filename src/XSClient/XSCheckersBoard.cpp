#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSColours.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSGlobals.h"
#include "XSClient/XSClientGame.h"
#include "XSClient/XSCheckersBoard.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSVertexAttributes.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSFont.h"

namespace XS {

	namespace ClientGame {

		void CheckersPiece::Move( uint8_t toOffset ) {
			SDL_assert( !"Unimplemented" );
		}

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

			// register the debug font
			font = Renderer::Font::Register( "menu" );

			// generate the board texture
			static const Renderer::VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_TexCoord" },
				{ 2, "in_Colour" },
			};
			program = new Renderer::ShaderProgram( "quad", "quad", attributes, ARRAY_LEN( attributes ) );
			material = new Renderer::Material();

			uint32_t outWidth = 0u, outHeight = 0u;
			uint8_t *boardData = Renderer::LoadPNG( "textures/checkers.png", &outWidth, &outHeight );
			boardTexture = new Renderer::Texture( outWidth, outHeight, Renderer::InternalFormat::RGBA8, boardData );
			delete boardData;
			Renderer::Material::SamplerBinding samplerBinding;
			samplerBinding.unit = 0;
			samplerBinding.uniform = "u_Texture";
			samplerBinding.texture = boardTexture;
			material->samplerBindings.push_back( samplerBinding );

			material->shaderProgram = program;
		}

		CheckersBoard::~CheckersBoard() {
			delete program;
			delete boardTexture;
			delete material;
		}

		void CheckersBoard::Update( real64_t dt ) {
			//TODO: check for kings
			//TODO: check end-game rules
		}

		void CheckersBoard::Render( real64_t dt ) const {
			const real32_t tileWidth = Renderer::state.window.width / 14.0f;
			const real32_t tileHeight = Renderer::state.window.height / 8.0f;

			const real32_t boardWidth = dimensions * tileWidth;
			const real32_t boardHeight = dimensions * tileHeight;

			// render the background
			Renderer::DrawQuad(
				0.0f, 0.0f,
				boardWidth, boardHeight,
				0.0f, 0.0f, 1.0f, 1.0f,
				nullptr,
				material
			);

			// render each piece
			for ( const auto &piece : pieces ) {
				const real32_t x = static_cast<real32_t>( piece.offset % dimensions ) * tileWidth;
				const real32_t y = static_cast<real32_t>( piece.offset / dimensions ) * tileHeight;
				const vector4 *colour = nullptr;
				if ( piece.colour == CheckersPiece::Colour::Red ) {
					colour = &colourTable[ColourIndex( COLOUR_RED )];
				}
				else {
					colour = &colourTable[ColourIndex( COLOUR_BLACK )];
				}
				Renderer::DrawQuad(
					x, y,
					tileWidth, tileHeight,
					0.0f, 0.0f, 1.0f, 1.0f,
					colour,
					nullptr
				);
			}

			// show the possible moves
			const vector4 *possibleColour = &colourTable[ColourIndex( COLOUR_YELLOW )];
			if ( selectedPiece ) {
				for ( auto &move : possibleMoves ) {
					const real32_t x = static_cast<real32_t>( move % dimensions ) * tileWidth;
					const real32_t y = static_cast<real32_t>( move / dimensions ) * tileHeight;
					Renderer::DrawQuad(
						x, y,
						tileWidth, tileHeight,
						0.0f, 0.0f, 1.0f, 1.0f,
						possibleColour,
						nullptr
					);
				}
			}

			// debug positional text
			if ( Common::com_developer->GetBool() ) {
				for ( size_t i = 0u; i < size; i++ ) {
					const real32_t x = static_cast<real32_t>( i % dimensions ) * tileWidth;
					const real32_t y = static_cast<real32_t>( i / dimensions ) * tileHeight;

					// debug text for "offset: row/col"
					const std::string posText = String::Format(
						"%i: %i/%i",
						i,
						i / dimensions,
						i % dimensions
					);
					const uint16_t posTextSize = 14u;
					real32_t textWidth = 0.0f;
					for ( const char *p = posText.c_str(); *p; p++ ) {
						textWidth += font->GetGlyphWidth( *p, posTextSize );
					}

					const vector2 pos( x, y );
					font->Draw( pos, posText, posTextSize, &colourTable[ColourIndex( COLOUR_PURPLE )] );
				}
			}
		}

		void CheckersBoard::MouseButtonEvent( real32_t cursorX, real32_t cursorY ) {
			const real32_t pieceWidth = Renderer::state.window.width / 14.0f;
			const real32_t pieceHeight = Renderer::state.window.height / 8.0f;

			if ( !selectedPiece ) {
				for ( auto &piece : pieces ) {
					const real32_t pieceX = static_cast<real32_t>( piece.offset % dimensions ) * pieceWidth;
					const real32_t pieceY = static_cast<real32_t>( piece.offset / dimensions ) * pieceHeight;
					if ( cursorX >= pieceX && cursorX < pieceX + pieceWidth
						&& cursorY >= pieceY && cursorY < pieceY + pieceHeight )
					{
						selectedPiece = &piece;
						CalculatePossibleMoves();
						break;
					}
				}
				return;
			}

			// bounds check
			if ( cursorX < 0.0f || cursorX >= dimensions * pieceWidth
				|| cursorY < 0.0f || cursorY >= dimensions * pieceHeight )
			{
				return;
			}

			// should be valid, try calculating the offset
			uint8_t col = cursorX / pieceWidth;
			uint8_t row = cursorY / pieceHeight;
			uint8_t offset = (row * dimensions) + col;
			for ( auto &move : possibleMoves ) {
				if ( offset == move ) {
					selectedPiece->offset = move;
					break;
				}
			}

			selectedPiece = nullptr;
			possibleMoves.clear();
		}

		void CheckersBoard::CalculatePossibleMoves( void ) {
			bool jumped[4] = {};
			int8_t moves[4] = {
				-9, -7,
				// us
				 7,  9
			};

			for ( size_t i = 0u; i < sizeof(moves); /*i++*/ ) {
				int8_t *move = &moves[i];

				if ( *move == 0u ) {
					// invalid, skip it
					i++;
					continue;
				}

				int8_t result = static_cast<int8_t>( selectedPiece->offset ) + *move;
				uint8_t moveCol = result % dimensions;
				uint8_t pieceCol = selectedPiece->offset % dimensions;
				uint8_t moveRow = result / dimensions;
				uint8_t pieceRow = selectedPiece->offset / dimensions;
				if ( result < 0 || result > size
					|| (std::abs( pieceCol - moveCol ) > 1 && !jumped[i])
					|| (std::abs( pieceRow - moveRow ) > 1 && !jumped[i]) )
				{
					*move = 0u; // mark it as invalid
				}
				else {
					for ( const auto &piece : pieces ) {
						if ( result == piece.offset ) {
							// desired position is occupied, see if we can jump over it
							bool canJump = !jumped[i] && piece.colour != selectedPiece->colour;
							int8_t jumpOffset = static_cast<int8_t>( selectedPiece->offset ) + (*move * 2);
							uint8_t jumpCol = jumpOffset % dimensions;
							bool jumpOccupied = false;
							for ( auto &jumpPiece : pieces ) {
								if ( jumpPiece.offset == jumpOffset ) {
									jumpOccupied = true;
									break;
								}
							}
							// alter the potential move and start the search again
							if ( !canJump || jumpOccupied || std::abs( pieceCol - jumpCol ) > 2 ) {
								*move = 0u;
							}
							else {
								*move *= 2u;
								jumped[i] = true;
							}
							i = 0u;
							continue;
						}
					}
				}

				i++;
			}

			for ( auto &move : moves ) {
				if ( move != 0 ) {
					possibleMoves.push_back( selectedPiece->offset + move );
				}
			}
			if ( possibleMoves.empty() ) {
				selectedPiece = nullptr;
			}
		}

	} // namespace ClientGame

} // namespace XS
