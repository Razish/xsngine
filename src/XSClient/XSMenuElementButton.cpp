#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSTokenParser.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSColours.h"
#include "XSCommon/XSEvent.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSMenuElementButton.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSFont.h"

namespace XS {

	namespace Client {

		void MenuElementButton::ParseProperties( TokenParser *parser, const char *fileName ) {
			const char *tok = nullptr;
			parser->ParseString( &tok );

			if ( String::CompareCase( tok, "{" ) ) {
				console.Print( PrintLevel::Normal,
					"%s missing opening brace when parsing properties for button menu element from %s:%i\n",
					XS_FUNCTION,
					fileName,
					parser->GetCurrentLine()
				);
				return;
			}

			while ( true ) {
				parser->ParseString( &tok );

				// see if we reached the end of the button definition
				if ( !String::CompareCase( tok, "}" ) ) {
					break;
				}

				// centered
				else if ( !String::Compare( tok, "centered" ) ) {
					properties.centered = true;
					parser->SkipLine();
				}
			}
		}

		MenuElementButton::MenuElementButton( TokenParser *parser, const char *fileName )
		: size{ 0.0f, 0.0f }, texture( "" ), cmd( "" ), text( "" ), pointSize( 0u )
		{
			std::memset( &properties, 0, sizeof(properties) );
			std::memset( &assets, 0, sizeof(assets) );

			const char *tok = nullptr;
			parser->ParseString( &tok );

			if ( String::CompareCase( tok, "{" ) ) {
				console.Print( PrintLevel::Normal,
					"%s missing opening brace when parsing button menu element from %s:%i\n",
					XS_FUNCTION,
					fileName,
					parser->GetCurrentLine()
				);
				return;
			}

			while ( true ) {
				parser->ParseString( &tok );

				// see if we reached the end of the button definition
				if ( !String::CompareCase( tok, "}" ) ) {
					break;
				}

				// properties
				else if ( !String::Compare( tok, "properties" ) ) {
					ParseProperties( parser, fileName );
					parser->SkipLine();
				}

				// name
				else if ( !String::Compare( tok, "name" ) ) {
					if ( parser->ParseString( &tok ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing name definition when parsing button menu element from %s:%i\n",
							XS_FUNCTION,
							fileName,
							parser->GetCurrentLine()
						);
					}
					else {
						name = tok;
					}
					parser->SkipLine();
				}

				// position
				else if ( !String::Compare( tok, "position" ) ) {
					real32_t x, y;
					if ( parser->ParseReal32( &x ) || parser->ParseReal32( &y ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing position definition when parsing button menu element from %s:%i\n",
							XS_FUNCTION,
							fileName,
							parser->GetCurrentLine()
						);
					}
					else {
						position[0] = x;
						position[1] = y;
					}
					parser->SkipLine();
				}

				// size
				else if ( !String::Compare( tok, "size" ) ) {
					real32_t w, h;
					if ( parser->ParseReal32( &w ) || parser->ParseReal32( &h ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing size definition when parsing button menu element from %s:%i\n",
							XS_FUNCTION,
							fileName,
							parser->GetCurrentLine()
						);
					}
					else {
						size[0] = w;
						size[1] = h;
					}
					parser->SkipLine();
				}

				// texture
				else if ( !String::Compare( tok, "texture" ) ) {
					if ( parser->ParseString( &tok ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing texture definition when parsing button menu element from %s:%i\n",
							XS_FUNCTION,
							fileName,
							parser->GetCurrentLine()
						);
						parser->SkipLine();
						continue;
					}
					else {
						texture = tok;
					}

					parser->SkipLine();
				}

				// command
				else if ( !String::Compare( tok, "cmd" ) ) {
					if ( parser->ParseString( &tok ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing command definition when parsing button menu element from %s:%i\n",
							XS_FUNCTION,
							fileName,
							parser->GetCurrentLine()
						);
						parser->SkipLine();
						continue;
					}
					else {
						cmd = tok;
					}

					parser->SkipLine();
				}

				// font + size
				else if ( !String::Compare( tok, "font" ) ) {
					if ( parser->ParseString( &tok ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing font definition when parsing button menu element from %s:%i\n",
							XS_FUNCTION,
							fileName,
							parser->GetCurrentLine()
						);
						parser->SkipLine();
						continue;
					}
					else {
						assets.font = Renderer::Font::Register( tok );
					}

					uint32_t tmpPointSize;
					if ( parser->ParseUInt32( &tmpPointSize ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing font size definition when parsing button menu element from %s:%i\n",
							XS_FUNCTION,
							fileName,
							parser->GetCurrentLine()
						);
					}
					else {
						pointSize = static_cast<uint16_t>( tmpPointSize );
					}

					parser->SkipLine();
				}

				// text to display
				else if ( !String::Compare( tok, "text" ) ) {
					if ( parser->ParseString( &tok ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing text definition when parsing button menu element from %s:%i\n",
							XS_FUNCTION,
							fileName,
							parser->GetCurrentLine()
						);
						parser->SkipLine();
						continue;
					}
					else {
						text = tok;
					}

					parser->SkipLine();
				}
			}
		}

		bool MenuElementButton::MouseWithinBounds( const vector2 &mousePos ) const {
			vector2 topLeft = position;
			if ( properties.centered ) {
				topLeft -= size / 2.0f;
			}
			if ( mousePos[0] > topLeft[0] && mousePos[0] < (topLeft[0] + size[0])
				&& mousePos[1] > topLeft[1] && mousePos[1] < (topLeft[1] + size[1]) )
			{
				return true;
			}
			return false;
		}


		void MenuElementButton::Paint( void ) {
			// lazy loading
			if ( !assets.background ) {
				Renderer::Material *material = new Renderer::Material();

				Renderer::Material::SamplerBinding samplerBinding = {};
				uint32_t bgWidth, bgHeight;
				uint8_t *bgData = Renderer::LoadPNG(
					texture.c_str(),
					&bgWidth, &bgHeight
				);
				samplerBinding.texture = new Renderer::Texture(
					bgWidth, bgHeight,
					Renderer::InternalFormat::RGBA8,
					bgData
				);
				material->samplerBindings.push_back( samplerBinding );

				material->shaderProgram = Renderer::quadProgram;

				assets.background = material;
				delete bgData;
			}

			const real32_t bgWidth = size[0] * Renderer::state.window.width;
			const real32_t bgHeight = size[1] * Renderer::state.window.height;
			const vector2 topLeft = {
				(position[0] * Renderer::state.window.width) - (bgWidth / 2.0f),
				(position[1] * Renderer::state.window.height) - (bgHeight / 2.0f)
			};

			Renderer::DrawQuad(
				topLeft[0], topLeft[1],
				bgWidth, bgHeight,
				0.0f, 0.0f, 1.0f, 1.0f,
				&colourTable[ColourIndex( COLOUR_WHITE )],
				assets.background
			);

			const size_t textLen = text.length();
			real32_t textWidth = 0.0f;
			for ( size_t i = 0u; i < textLen; i++ ) {
				textWidth += assets.font->GetGlyphWidth( text[i], pointSize );
			}
			const vector2 textSize = {
				textWidth,
				assets.font->lineHeight[pointSize]
			};
			const vector2 bgMidPoint = {
				topLeft[0] + ((size[0] * Renderer::state.window.width) / 2.0f),
				topLeft[1] + ((size[1] * Renderer::state.window.height) / 2.0f)
			};

			// draw the text in the middle of the button
			assets.font->Draw(
				bgMidPoint - (textSize / 2.0f),
				text,
				pointSize,
				&colourTable[ColourIndex(COLOUR_RED)]
			);
		}

		void MenuElementButton::MouseButtonEvent( const struct MouseButtonEvent &ev ) {
			if ( !MouseWithinBounds( Client::cursorPos ) ) {
				return;
			}

			if ( ev.pressed && (ev.button == SDL_BUTTON_LEFT) ) {
				Command::Append( cmd.c_str() );
			}
		}

		void MenuElementButton::MouseMotionEvent( void ) {
			// ...
		}

	} // namespace Client

} // namespace XS
