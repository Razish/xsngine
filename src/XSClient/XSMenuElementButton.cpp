#include <algorithm>

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
				if ( !String::Compare( tok, "centered" ) ) {
					properties.centered = true;
					parser->SkipLine();
				}

				// vertical
				else if ( !String::Compare( tok, "vertical" ) ) {
					properties.vertical = true;
					parser->SkipLine();
				}
			}
		}

		MenuElementButton::MenuElementButton( TokenParser *parser, const char *fileName )
		: texture( "" ), cmd( "" )
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

				// name
				if ( !String::Compare( tok, "name" ) ) {
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
						position.x = x;
						position.y = y;
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
						size.w = w;
						size.h = h;
					}
					parser->SkipLine();
				}

				// command
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

			const vector2 topLeft(
				position.x * Renderer::state.window.width,
				position.y * Renderer::state.window.height
			);
			const real32_t bgWidth = size.x * Renderer::state.window.width;
			const real32_t bgHeight = size.y * Renderer::state.window.height;

			Renderer::DrawQuad(
				topLeft.x, topLeft.y,
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
			const vector2 textSize(
				textWidth,
				assets.font->lineHeight[pointSize]
			);
			const vector2 bgMidPoint(
				topLeft.x + ((size.x * Renderer::state.window.width) / 2.0f),
				topLeft.y + ((size.y * Renderer::state.window.height) / 2.0f)
			);

			// draw the text in the middle of the button
			assets.font->Draw(
				bgMidPoint - (textSize / 2.0f),
				text,
				pointSize,
				&colourTable[ColourIndex(COLOUR_RED)]
			);
		}

		void MenuElementButton::MouseButtonEvent( const struct MouseButtonEvent &ev, const vector2 &cursorPos ) {
			if ( ev.pressed && (ev.button == SDL_BUTTON_LEFT) ) {
				Command::Append( cmd.c_str() );
			}
		}

		void MenuElementButton::MouseMotionEvent( const vector2 &cursorPos ) {
			// ...
		}

	} // namespace Client

} // namespace XS
