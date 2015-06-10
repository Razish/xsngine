#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSColours.h"
#include "XSCommon/XSString.h"
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

		void CheckersPiece::Move( uint8_t x, uint8_t y ) {
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
			// ...
		}

		void CheckersBoard::Render( real64_t dt ) {
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
			for ( CheckersPiece &piece : pieces ) {
				real32_t x = static_cast<real32_t>( piece.offset % dimensions ) * tileWidth;
				real32_t y = static_cast<real32_t>( piece.offset / dimensions ) * tileHeight;
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

				// debug text for "offset: row/col"
				std::string posText = String::Format(
					"%i: %i/%i",
					piece.offset,
					piece.offset / dimensions,
					piece.offset % dimensions
				);
				const uint16_t posTextSize = 14u;
				real32_t textWidth = 0.0f;
				for ( const char *p = posText.c_str(); *p; p++ ) {
					textWidth += font->GetGlyphWidth( *p, posTextSize );
				}

				vector2 pos( x, y );
				font->Draw( pos, posText, posTextSize, &colourTable[ColourIndex( COLOUR_GREEN )] );
			}
		}

	} // namespace ClientGame

} // namespace XS
