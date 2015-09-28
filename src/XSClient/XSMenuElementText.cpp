#include <algorithm>

#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSTokenParser.h"
#include "XSCommon/XSColours.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSCvar.h"
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
				else if ( !String::Compare( tok, "centered" ) ) {
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

				// properties
				else if ( !String::Compare( tok, "properties" ) ) {
					ParseProperties( parser, fileName );
					parser->SkipLine();
				}

				// name
				else if ( !String::Compare( tok, "name" ) ) {
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
						position[0] = x;
						position[1] = y;
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
			if ( text.empty() ) {
				return;
			}

			const vector2 topLeft = {
				position[0] * Renderer::rdState.window.width,
				position[1] * Renderer::rdState.window.height
			};

			std::string displayText;
			if ( text[0] == '$' ) {
				Cvar *cv = Cvar::Get( text.substr( 1 ) );
				if ( !cv ) {
					return;
				}
				displayText = cv->GetFullString();
			}
			else {
				displayText = text;
			}

			const size_t textLen = displayText.length();
			real32_t textWidth = 0.0f;
			for ( size_t i = 0u; i < textLen; i++ ) {
				textWidth += assets.font->GetGlyphWidth( displayText[i], pointSize );
			}
			const vector2 textSize = {
				textWidth,
				assets.font->lineHeight[pointSize]
			};

			// draw the text
			assets.font->Draw(
				properties.centered
					? topLeft - (textSize / 2.0f)
					: topLeft,
				displayText,
				pointSize,
				&colourTable[ColourIndex(COLOUR_WHITE)]
			);
		}

		void MenuElementText::MouseButtonEvent( const struct MouseButtonEvent &ev ) {
			// ...
		}

		void MenuElementText::MouseMotionEvent( void ) {
			// ...
		}

	} // namespace Client

} // namespace XS
