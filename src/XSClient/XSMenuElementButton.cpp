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
#include "XSClient/XSMenuElementTextInput.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSFont.h"

namespace Client {

	MenuElementButton::MenuElementButton( const Menu &parent, TokenParser *parser, const char *fileName )
	:	MenuElement( parent )
	{
		const char *tok = nullptr;
		if ( parser->ParseString( &tok ) || String::CompareCase( tok, "{" ) ) {
			console.Print( PrintLevel::Normal,
				"%s missing opening brace when parsing button menu element from %s:%i\n",
				XS_FUNCTION,
				fileName,
				parser->GetCurrentLine()
			);
			return;
		}

		while ( true ) {
			// see if we reached the end of the button definition
			if ( parser->ParseString( &tok ) || !String::CompareCase( tok, "}" ) ) {
				break;
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
					common.name = tok;
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
						"%s missing size definition when parsing button menu element from %s:%i\n",
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
						"%s missing texture definition when parsing button menu element from %s:%i\n",
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
					properties.cmd = tok;
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
					data.font = Renderer::Font::Register( tok );
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
					properties.pointSize = static_cast<uint16_t>( tmpPointSize );
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
					properties.text = tok;
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

	void MenuElementButton::Paint( void ) {
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

		const size_t textLen = properties.text.length();
		real32_t textWidth = 0.0f;
		for ( size_t i = 0u; i < textLen; i++ ) {
			textWidth += data.font->GetGlyphWidth( properties.text[i], properties.pointSize );
		}
		const vector2 textSize = {
			textWidth,
			data.font->lineHeight[properties.pointSize]
		};
		const vector2 bgMidPoint = {
			topLeft[0] + ((properties.size[0] * parent.view.width) / 2.0f),
			topLeft[1] + ((properties.size[1] * parent.view.height) / 2.0f)
		};

		// draw the text in the middle of the button
		data.font->Draw(
			bgMidPoint - (textSize / 2.0f),
			properties.text,
			properties.pointSize,
			&colourTable[ColourIndex(COLOUR_RED)]
		);
	}

	bool MenuElementButton::KeyboardEvent( const struct KeyboardEvent &ev ) {
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

		if ( ev.down && ev.key == SDLK_RETURN ) {
			Command::Append( properties.cmd.c_str() );
			return true;
		}

		return false;
	}

	bool MenuElementButton::MouseButtonEvent( const struct MouseButtonEvent &ev ) {
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
			// replace all tokens starting with $ with the data from the MenuElementTextInput
			std::vector<std::string> tokens;
			String::Split( properties.cmd, ' ', tokens );
			for ( auto &token : tokens ) {
				if ( token[0] == '$' ) {
					const char *toFind = token.c_str() + 1;
					auto *found = parent.Find( toFind );
					if ( found ) {
						auto *inputElement = dynamic_cast<MenuElementTextInput *>( found );
						if ( inputElement ) {
							if ( inputElement->data.inputText.empty() ) {
								console.Print( PrintLevel::Debug, "%s(%s): input element \"%s\" is empty\n",
									XS_FUNCTION,
									common.name.c_str(),
									found->common.name.c_str()
								);
								continue;
							}
							token = inputElement->data.inputText;
							continue;
						}
					}
					else {
						console.Print( PrintLevel::Debug, "%s(%s): could not find input element named \"%s\"\n",
							XS_FUNCTION,
							common.name.c_str(),
							toFind
						);
					}
				}
			}

			// execute the associated command
			std::string finalCmd;
			String::Join( tokens, " ", finalCmd );
			Command::Append( finalCmd.c_str() );
			return true;
		}

		return false;
	}

	bool MenuElementButton::MouseMotionEvent( const struct MouseMotionEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		//TODO: highlighting buttons or such

		return false;
	}

	bool MenuElementButton::MouseWheelEvent( const struct MouseWheelEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		return false;
	}

} // namespace Client
