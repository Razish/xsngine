#include <algorithm>

#include <SDL2/SDL.h>

#include "Common/Common.h"
#include "Common/String.h"
#include "Common/Console.h"
#include "Common/TokenParser.h"
#include "Common/Cvar.h"
#include "Common/Colours.h"
#include "Common/Event.h"
#include "Client/Client.h"
#include "Client/MenuElementSlider.h"
#include "Renderer/Material.h"
#include "Renderer/ImagePNG.h"
#include "Renderer/InternalFormat.h"
#include "Renderer/Texture.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Font.h"

namespace Client {

	MenuElementSlider::MenuElementSlider( const Menu &parent, TokenParser *parser, const char *fileName )
	:	MenuElement( parent )
	{
		const char *tok = nullptr;
		if ( parser->ParseString( &tok ) || String::CompareCase( tok, "{" ) ) {
			console.Print( PrintLevel::Normal,
				"%s missing opening brace when parsing slider menu element from %s:%i\n",
				XS_FUNCTION,
				fileName,
				parser->GetCurrentLine()
			);
			return;
		}

		while ( true ) {
			// see if we reached the end of the slider definition
			if ( parser->ParseString( &tok ) || !String::CompareCase( tok, "}" ) ) {
				break;
			}

			// tooltip
			else if ( !String::Compare( tok, "tooltip" ) ) {
				ParseTooltip( parser, fileName );
				parser->SkipLine();
			}

			// name
			else if ( !String::Compare( tok, "name" ) ) {
				if ( parser->ParseString( &tok ) ) {
					console.Print( PrintLevel::Normal,
						"%s missing name definition when parsing slider menu element from %s:%i\n",
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
						"%s missing position definition when parsing slider menu element from %s:%i\n",
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
						"%s missing size definition when parsing slider menu element from %s:%i\n",
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

			// cvar
			else if ( !String::Compare( tok, "cvar" ) ) {
				if ( parser->ParseString( &tok ) ) {
					console.Print( PrintLevel::Normal,
						"%s missing cvar definition when parsing slider menu element from %s:%i\n",
						XS_FUNCTION,
						fileName,
						parser->GetCurrentLine()
					);
					parser->SkipLine();
					continue;
				}
				else {
					properties.cvarName = tok;
				}

				real32_t bottom, top;
				if ( parser->ParseReal32( &bottom ) || parser->ParseReal32( &top ) ) {
					console.Print( PrintLevel::Normal,
						"%s missing range definition when parsing slider menu element from %s:%i\n",
						XS_FUNCTION,
						fileName,
						parser->GetCurrentLine()
					);
					parser->SkipLine();
					continue;
				}
				else {
					properties.range.bottom = bottom;
					properties.range.top = top;
				}
				parser->SkipLine();
			}

			// post-exec command
			else if ( !String::Compare( tok, "postExec" ) ) {
				if ( parser->ParseString( &tok ) ) {
					console.Print( PrintLevel::Normal,
						"%s missing postExec definition when parsing slider menu element from %s:%i\n",
						XS_FUNCTION,
						fileName,
						parser->GetCurrentLine()
					);
					parser->SkipLine();
					continue;
				}
				else {
					properties.postExecCommand = tok;
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

			// integral values
			else if ( !String::Compare( tok, "integral" ) ) {
				properties.integral = true;
				parser->SkipLine();
			}
		}
	}

	void MenuElementSlider::Paint( void ) {
		if ( common.hidden ) {
			return;
		}

		// lazy loading
		if ( !data.thumb ) {
			Renderer::Material *material = new Renderer::Material();

			Renderer::Material::SamplerBinding samplerBinding = {};
			uint32_t thumbWidth, thumbHeight;
			uint8_t *thumbData = Renderer::LoadPNG(
				"textures/ui/slider_thumb.png",
				&thumbWidth, &thumbHeight
			);
			samplerBinding.texture = new Renderer::Texture(
				thumbWidth, thumbHeight,
				Renderer::InternalFormat::RGBA8,
				thumbData
			);
			material->samplerBindings.push_back( samplerBinding );

			material->shaderProgram = Renderer::quadProgram;

			data.thumb = material;
			delete thumbData;
		}
		if ( !data.bar ) {
			Renderer::Material *material = new Renderer::Material();

			Renderer::Material::SamplerBinding samplerBinding = {};
			uint32_t barWidth, barHeight;
			uint8_t *barData = Renderer::LoadPNG(
				"textures/ui/slider_bar.png",
				&barWidth, &barHeight
			);
			samplerBinding.texture = new Renderer::Texture(
				barWidth, barHeight,
				Renderer::InternalFormat::RGBA8,
				barData
			);
			material->samplerBindings.push_back( samplerBinding );

			material->shaderProgram = Renderer::quadProgram;

			data.bar = material;
			delete barData;
		}

		const vector2 topLeft {
			common.position[0] * parent.view.width,
			common.position[1] * parent.view.height
		};
		const real32_t barWidth = properties.size[0] * parent.view.width;
		const real32_t barHeight = properties.size[1] * parent.view.height;
		const real32_t thumbSize = std::min( barWidth, barHeight );

		Renderer::DrawQuad(
			topLeft[0], topLeft[1],
			barWidth, barHeight,
			0.0f, 0.0f, 1.0f, 1.0f,
			colourTable[ColourIndex( COLOUR_WHITE )],
			data.bar
		);

		Cvar *cvar = Cvar::Get( properties.cvarName );
		if ( cvar ) {
			// draw the default value
			real32_t r = static_cast<real32_t>( atof( cvar->GetDefaultString().c_str() ) );
			real32_t v = std::min( properties.range.top, std::max( properties.range.bottom, r ) );
			real32_t d = properties.range.top - properties.range.bottom;
			real32_t f = (v - properties.range.bottom) / d;
			Renderer::DrawQuad(
				topLeft[0] + (barWidth * f) - (thumbSize / 2.0f), topLeft[1],
				thumbSize, thumbSize,
				0.0f, 0.0f, 1.0f, 1.0f,
				colourTable[ColourIndex( COLOUR_GREY )],
				data.thumb
			);

			// find the fraction of the value in the range and draw the thumb handle
			r = cvar->GetReal32();
			v = std::min( properties.range.top, std::max( properties.range.bottom, r ) );
			d = properties.range.top - properties.range.bottom;
			f = (v - properties.range.bottom) / d;
			Renderer::DrawQuad(
				topLeft[0] + (barWidth * f) - (thumbSize / 2.0f), topLeft[1],
				thumbSize, thumbSize,
				0.0f, 0.0f, 1.0f, 1.0f,
				colourTable[ColourIndex( COLOUR_WHITE )],
				data.thumb
			);
		}
	}

	void MenuElementSlider::UpdateValue( real32_t frac ) {
		Cvar *cvar = Cvar::Get( properties.cvarName );
		if ( !cvar ) {
			return;
		}

		// scale the fractional value up to the real value based on the range of the slider
		const real32_t d = properties.range.top - properties.range.bottom;
		real32_t desiredValue = (frac * d) + properties.range.bottom;

		if ( properties.integral ) {
			cvar->Set( static_cast<uint32_t>( std::round( desiredValue ) ) );
		}
		else {
			cvar->Set( desiredValue );
		}
	}

	bool MenuElementSlider::KeyboardEvent( const struct KeyboardEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		return false;
	}

	bool MenuElementSlider::MouseButtonEvent( const struct MouseButtonEvent &ev ) {
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
		Cvar *cvar = Cvar::Get( properties.cvarName );
		if ( !cvar ) {
			return false;
		}

		if ( ev.pressed && (ev.button == SDL_BUTTON_LEFT) ) {
			// find the fractional point we clicked at
			const real32_t cursorX = Client::cursorPos[0];
			const real32_t realPos = cursorX - common.position[0];
			const real32_t f = realPos / properties.size[0];

			UpdateValue( f );
			data.updatingValue = true;
			return true;
		}
		else {
			//FIXME: can we move updatingValue outside this scope?
			data.updatingValue = false;
			if ( !properties.postExecCommand.empty() ) {
				Command::Append( properties.postExecCommand.c_str() );
				return true;
			}
		}

		return false;
	}

	bool MenuElementSlider::MouseMotionEvent( const struct MouseMotionEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		const bool mouseWithinBounds = vector2::PointWithinBounds(
			properties.centered ? common.position - properties.size / 2.0f : common.position,
			Client::cursorPos,
			properties.size
		);
		if ( mouseWithinBounds ) {
			tooltip.mouseHovering = true;
			tooltip.lastMousePos = Client::cursorPos;
		}
		else {
			tooltip.mouseHovering = false;
			if ( data.updatingValue ) {
				const real32_t cursorX = Client::cursorPos[0];
				const real32_t realPos = cursorX - common.position[0];
				const real32_t f = realPos / properties.size[0];
				UpdateValue( f );
				if ( !properties.postExecCommand.empty() ) {
					Command::Append( properties.postExecCommand.c_str() );
				}
			}
			data.updatingValue = false;
			return false;
		}

		if ( data.updatingValue ) {
			// find the fractional point we clicked at
			const real32_t cursorX = Client::cursorPos[0];
			const real32_t realPos = cursorX - common.position[0];
			const real32_t f = realPos / properties.size[0];

			UpdateValue( f );
		}

		return false;
	}

	bool MenuElementSlider::MouseWheelEvent( const struct MouseWheelEvent &ev ) {
		if ( common.decorative ) {
			return false;
		}

		return false;
	}

} // namespace Client
