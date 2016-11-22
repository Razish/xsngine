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

namespace Client {

	MenuElementText::MenuElementText( const Menu &parent, TokenParser *parser, const char *fileName )
	:	MenuElement( parent )
	{
		common.decorative = true;

		const char *tok = nullptr;
		if ( parser->ParseString( &tok ) || String::CompareCase( tok, "{" ) ) {
			console.Print( PrintLevel::Normal,
				"%s missing opening brace when parsing text menu element from %s:%i\n",
				XS_FUNCTION,
				fileName,
				parser->GetCurrentLine()
			);
			return;
		}

		while ( true ) {
			// see if we reached the end of the text definition
			if ( parser->ParseString( &tok ) || !String::CompareCase( tok, "}" ) ) {
				break;
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
					common.name = tok;
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
					common.position[0] = x;
					common.position[1] = y;
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
					data.font = Renderer::Font::Register( tok );
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
					properties.pointSize = static_cast<uint16_t>( tmpPointSize );
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
					properties.text = tok;
				}

				parser->SkipLine();
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

	void MenuElementText::Paint( void ) {
		if ( common.hidden ) {
			return;
		}

		if ( properties.text.empty() ) {
			return;
		}

		const vector2 topLeft = {
			common.position[0] * parent.view.width,
			common.position[1] * parent.view.height
		};

		std::string displayText;
		if ( properties.text[0] == '$' ) {
			const Cvar *cv = Cvar::Get( properties.text.substr( 1 ) );
			if ( !cv ) {
				return;
			}
			displayText = cv->GetFullString();
		}
		else {
			displayText = properties.text;
		}

		const size_t textLen = displayText.length();
		real32_t textWidth = 0.0f;
		for ( size_t i = 0u; i < textLen; i++ ) {
			textWidth += data.font->GetGlyphWidth( displayText[i], properties.pointSize );
		}
		const vector2 textSize = {
			textWidth,
			data.font->lineHeight[properties.pointSize]
		};

		// draw the text
		data.font->Draw(
			properties.centered
				? topLeft - (textSize / 2.0f)
				: topLeft,
			displayText,
			properties.pointSize,
			&colourTable[ColourIndex(COLOUR_WHITE)]
		);
	}

	bool MenuElementText::KeyboardEvent( const struct KeyboardEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		return false;
	}

	bool MenuElementText::MouseButtonEvent( const struct MouseButtonEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		return false;
	}

	bool MenuElementText::MouseMotionEvent( const struct MouseMotionEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		return false;
	}

	bool MenuElementText::MouseWheelEvent( const struct MouseWheelEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		return false;
	}

} // namespace Client
