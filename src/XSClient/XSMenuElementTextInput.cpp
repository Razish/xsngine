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
#include "XSClient/XSMenuElementTextInput.h"
#include "XSInput/XSKeys.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSFont.h"

namespace Client {

	void MenuElementTextInput::Clear( void ) {
		// clear the current state (input line, cursor position, etc)
		data.inputText.clear();
		data.numChars = 0u;
		data.cursorPos = 0u;
	}

	MenuElementTextInput::MenuElementTextInput( const Menu &parent, TokenParser *parser, const char *fileName )
	:	MenuElement( parent )
	{
		const char *tok = nullptr;
		if ( parser->ParseString( &tok ) || String::CompareCase( tok, "{" ) ) {
			console.Print( PrintLevel::Normal,
				"%s missing opening brace when parsing text input menu element from %s:%i\n",
				XS_FUNCTION,
				fileName,
				parser->GetCurrentLine()
			);
			return;
		}

		while ( true ) {
			// see if we reached the end of the text input definition
			if ( parser->ParseString( &tok ) || !String::CompareCase( tok, "}" ) ) {
				break;
			}

			// name
			else if ( !String::Compare( tok, "name" ) ) {
				if ( parser->ParseString( &tok ) ) {
					console.Print( PrintLevel::Normal,
						"%s missing name definition when parsing text input menu element from %s:%i\n",
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
						"%s missing position definition when parsing text input menu element from %s:%i\n",
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

			// size
			else if ( !String::Compare( tok, "size" ) ) {
				real32_t w, h;
				if ( parser->ParseReal32( &w ) || parser->ParseReal32( &h ) ) {
					console.Print( PrintLevel::Normal,
						"%s missing size definition when parsing text input menu element from %s:%i\n",
						XS_FUNCTION,
						fileName,
						parser->GetCurrentLine()
					);
				}
				else {
					properties.size[0] = w;
					properties.size[1] = h;
				}
				parser->SkipLine();
			}

			// texture
			else if ( !String::Compare( tok, "texture" ) ) {
				if ( parser->ParseString( &tok ) ) {
					console.Print( PrintLevel::Normal,
						"%s missing texture definition when parsing text input menu element from %s:%i\n",
						XS_FUNCTION,
						fileName,
						parser->GetCurrentLine()
					);
					parser->SkipLine();
					continue;
				}
				else {
					properties.texture = tok;
				}

				parser->SkipLine();
			}

			// font + size
			else if ( !String::Compare( tok, "font" ) ) {
				if ( parser->ParseString( &tok ) ) {
					console.Print( PrintLevel::Normal,
						"%s missing font definition when parsing text input menu element from %s:%i\n",
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
						"%s missing font size definition when parsing text input menu element from %s:%i\n",
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

			// centered
			else if ( !String::Compare( tok, "centered" ) ) {
				properties.centered = true;
				parser->SkipLine();
			}
		}
	}

	void MenuElementTextInput::Paint( void ) {
		if ( common.hidden ) {
			return;
		}

		// lazy loading
		if ( !data.background ) {
			Renderer::Material *material = new Renderer::Material();

			Renderer::Material::SamplerBinding samplerBinding = {};
			uint32_t bgWidth, bgHeight;
			uint8_t *bgData = Renderer::LoadPNG(
				properties.texture.c_str(),
				&bgWidth, &bgHeight
			);
			samplerBinding.texture = new Renderer::Texture(
				bgWidth, bgHeight,
				Renderer::InternalFormat::RGBA8,
				bgData
			);
			material->samplerBindings.push_back( samplerBinding );

			material->shaderProgram = Renderer::quadProgram;

			data.background = material;
			delete[] bgData;
		}

		const real32_t bgWidth = properties.size[0] * parent.view.width;
		const real32_t bgHeight = properties.size[1] * parent.view.height;
		const vector2 topLeft = {
			(common.position[0] * parent.view.width) - (bgWidth / 2.0f),
			(common.position[1] * parent.view.height) - (bgHeight / 2.0f)
		};

		Renderer::DrawQuad(
			topLeft[0], topLeft[1],
			bgWidth, bgHeight,
			0.0f, 0.0f, 1.0f, 1.0f,
			colourTable[ColourIndex( COLOUR_WHITE )],
			data.background
		);

		if ( data.inputText.empty() ) {
			return;
		}
		const size_t textLen = data.inputText.length();
		real32_t textWidth = 0.0f;
		for ( size_t i = 0u; i < textLen; i++ ) {
			textWidth += data.font->GetGlyphWidth( data.inputText[i], properties.pointSize );
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
			data.inputText,
			properties.pointSize,
			&colourTable[ColourIndex(COLOUR_WHITE)]
		);
	}

	bool MenuElementTextInput::KeyboardEvent( const struct KeyboardEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		if ( !ev.down ) {
			return true;
		}

		char c = GetPrintableCharForKeycode( ev.key );
		if ( c >= 0x20 ) {
			// insert a character at the cursor and move the cursor along
			data.inputText.insert( data.cursorPos, 1, c );
			data.numChars++;
			data.cursorPos++;
		}
		else if ( ev.key == SDLK_BACKSPACE ) {
			// remove the character before the cursor
			if ( data.cursorPos != 0u ) {
				data.inputText.erase( data.cursorPos - 1, 1 );
				data.numChars--;
				data.cursorPos--;
			}
		}
		else if ( ev.key == SDLK_DELETE ) {
			// remove the character after the cursor
			if ( data.cursorPos != data.inputText.length() ) {
				data.inputText.erase( data.cursorPos, 1 );
				data.numChars--;
			}
		}
		else if ( ev.key == SDLK_RETURN ) {
			// commit the current line to history, pass it to the command buffer and print it to console

			// no action to take if it's empty...
			if ( data.inputText.empty() ) {
				return true;
			}

			// clear the state
			Clear();
		}
		else if ( ev.key == SDLK_LEFT ) {
			// move cursor left
			if ( data.cursorPos > 0 ) {
				data.cursorPos--;
			}
		}
		else if ( ev.key == SDLK_RIGHT ) {
			// move cursor right
			if ( data.cursorPos < data.numChars ) {
				data.cursorPos++;
			}
		}
		else if ( ev.key == SDLK_UP ) {
			//TODO: home?
		}
		else if ( ev.key == SDLK_DOWN ) {
			//TODO: end?
		}

		return false;
	}

	bool MenuElementTextInput::MouseButtonEvent( const struct MouseButtonEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		const bool mouseWithinBounds = vector2::PointWithinBounds(
			properties.centered ? common.position - properties.size / 2.0f : common.position,
			Client::cursorPos,
			properties.size
		);
		if ( !mouseWithinBounds ) {
			return false;
		}

		if ( ev.pressed && ev.button == SDL_BUTTON_LEFT ) {
			//TODO: advanced text selection
			return true;
		}

		return false;
	}

	bool MenuElementTextInput::MouseMotionEvent( const struct MouseMotionEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		return false;
	}

	bool MenuElementTextInput::MouseWheelEvent( const struct MouseWheelEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		return false;
	}

} // namespace Client
