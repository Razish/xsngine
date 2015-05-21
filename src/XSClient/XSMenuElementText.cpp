#include <algorithm>

#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSTokenParser.h"
#include "XSCommon/XSColours.h"
#include "XSCommon/XSEvent.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSMenuElementText.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSFont.h"

namespace XS {

	namespace Client {

		void MenuElementText::ParseProperties( TokenParser *parser, const char *fileName ) {
			const char *tok = nullptr;
			parser->ParseString( &tok );

			if ( String::CompareCase( tok, "{" ) ) {
				console.Print( PrintLevel::Normal,
					"%s missing opening brace when parsing properties for text menu element from %s:%i\n",
					XS_FUNCTION,
					fileName,
					parser->GetCurrentLine()
				);
				return;
			}

			while ( true ) {
				parser->ParseString( &tok );

				// see if we reached the end of the text definition
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

		MenuElementText::MenuElementText( TokenParser *parser, const char *fileName )
		: text( "" ), pointSize( 0u )
		{
			std::memset( &properties, 0, sizeof(properties) );
			std::memset( &assets, 0, sizeof(assets) );

			MenuElement::properties.decorative = true;

			const char *tok = nullptr;
			parser->ParseString( &tok );

			if ( String::CompareCase( tok, "{" ) ) {
				console.Print( PrintLevel::Normal,
					"%s missing opening brace when parsing text menu element from %s:%i\n",
					XS_FUNCTION,
					fileName,
					parser->GetCurrentLine()
				);
				return;
			}

			while ( true ) {
				parser->ParseString( &tok );

				// see if we reached the end of the text definition
				if ( !String::CompareCase( tok, "}" ) ) {
					break;
				}

				// name
				if ( !String::Compare( tok, "name" ) ) {
					if ( parser->ParseString( &tok ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing name definition when parsing text menu element from %s:%i\n",
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
							"%s missing position definition when parsing text menu element from %s:%i\n",
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

				// font + size
				else if ( !String::Compare( tok, "font" ) ) {
					if ( parser->ParseString( &tok ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing font definition when parsing text menu element from %s:%i\n",
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
							"%s missing font size definition when parsing text menu element from %s:%i\n",
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
							"%s missing text definition when parsing text menu element from %s:%i\n",
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

		void MenuElementText::Paint( void ) {
			const vector2 topLeft(
				position.x * Renderer::state.window.width,
				position.y * Renderer::state.window.height
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
			const vector2 midPoint(
				topLeft.x + (textSize.x / 2.0f),
				topLeft.y + (textSize.y / 2.0f)
			);

			// draw the text
			assets.font->Draw(
				properties.centered
					? midPoint - (textSize / 2.0f)
					: topLeft,
				text,
				pointSize,
				&colourTable[ColourIndex(COLOUR_RED)]
			);
		}

		void MenuElementText::MouseButtonEvent( const struct MouseButtonEvent &ev, const vector2 &cursorPos ) {
			// ...
		}

		void MenuElementText::MouseMotionEvent( const vector2 &cursorPos ) {
			// ...
		}

	} // namespace Client

} // namespace XS
