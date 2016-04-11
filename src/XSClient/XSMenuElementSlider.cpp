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
#include "XSClient/XSMenuElementSlider.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSFont.h"

namespace XS {

	namespace Client {

		void MenuElementSlider::ParseProperties( TokenParser *parser, const char *fileName ) {
			const char *tok = nullptr;

			if ( parser->ParseString( &tok ) || String::CompareCase( tok, "{" ) ) {
				console.Print( PrintLevel::Normal,
					"%s missing opening brace when parsing properties for slider menu element from %s:%i\n",
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

		MenuElementSlider::MenuElementSlider( const Menu &parent, TokenParser *parser, const char *fileName )
		: MenuElement( parent ), cvarName( "" ), postExecCommand( "" ), updatingValue( false )
		{
			size = { 0.0f, 0.0f };
			std::memset( &assets, 0, sizeof(assets) );
			std::memset( &range, 0, sizeof(range) );
			std::memset( static_cast<void *>( &properties ), 0, sizeof(properties) );

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

				// properties
				else if ( !String::Compare( tok, "properties" ) ) {
					ParseProperties( parser, fileName );
					parser->SkipLine();
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
						name = tok;
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
							"%s missing size definition when parsing slider menu element from %s:%i\n",
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
						cvarName = tok;
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
						range.bottom = bottom;
						range.top = top;
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
						postExecCommand = tok;
					}

					parser->SkipLine();
				}
			}
		}

		bool MenuElementSlider::MouseWithinBounds( const vector2 &mousePos ) const {
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

		void MenuElementSlider::Paint( void ) {
			if ( properties.hidden ) {
				return;
			}

			// lazy loading
			if ( !assets.thumb ) {
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

				assets.thumb = material;
				delete thumbData;
			}
			if ( !assets.bar ) {
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

				assets.bar = material;
				delete barData;
			}

			const vector2 topLeft {
				position[0] * parent.view.width,
				position[1] * parent.view.height
			};
			const real32_t barWidth = size[0] * parent.view.width;
			const real32_t barHeight = size[1] * parent.view.height;
			const real32_t thumbSize = std::min( barWidth, barHeight );

			Renderer::DrawQuad(
				topLeft[0], topLeft[1],
				barWidth, barHeight,
				0.0f, 0.0f, 1.0f, 1.0f,
				&colourTable[ColourIndex( COLOUR_WHITE )],
				assets.bar
			);

			Cvar *cvar = Cvar::Get( cvarName );
			if ( cvar ) {
				// draw the default value
				real32_t r = static_cast<real32_t>( atof( cvar->GetDefaultString().c_str() ) );
				real32_t v = std::min( range.top, std::max( range.bottom, r ) );
				real32_t d = range.top - range.bottom;
				real32_t f = (v - range.bottom) / d;
				Renderer::DrawQuad(
					topLeft[0] + (barWidth * f) - (thumbSize / 2.0f), topLeft[1],
					thumbSize, thumbSize,
					0.0f, 0.0f, 1.0f, 1.0f,
					&colourTable[ColourIndex( COLOUR_GREY )],
					assets.thumb
				);

				// find the fraction of the value in the range and draw the thumb handle
				r = cvar->GetReal32();
				v = std::min( range.top, std::max( range.bottom, r ) );
				d = range.top - range.bottom;
				f = (v - range.bottom) / d;
				Renderer::DrawQuad(
					topLeft[0] + (barWidth * f) - (thumbSize / 2.0f), topLeft[1],
					thumbSize, thumbSize,
					0.0f, 0.0f, 1.0f, 1.0f,
					&colourTable[ColourIndex( COLOUR_WHITE )],
					assets.thumb
				);
			}
		}

		void MenuElementSlider::UpdateValue( real32_t frac ) {
			Cvar *cvar = Cvar::Get( cvarName );
			if ( !cvar ) {
				return;
			}

			// scale the fractional value up to the real value based on the range of the slider
			const real32_t d = range.top - range.bottom;
			real32_t desiredValue = (frac * d) + range.bottom;

			if ( properties.integral ) {
				cvar->Set( static_cast<uint32_t>( std::round( desiredValue ) ) );
			}
			else {
				cvar->Set( desiredValue );
			}
		}

		bool MenuElementSlider::KeyboardEvent( const struct KeyboardEvent &ev ) {
			if ( properties.decorative ) {
				return false;
			}

			return false;
		}

		bool MenuElementSlider::MouseButtonEvent( const struct MouseButtonEvent &ev ) {
			if ( properties.decorative ) {
				return false;
			}

			if ( !MouseWithinBounds( Client::cursorPos ) ) {
				return false;
			}
			Cvar *cvar = Cvar::Get( cvarName );
			if ( !cvar ) {
				return false;
			}

			if ( ev.pressed && (ev.button == SDL_BUTTON_LEFT) ) {
				// find the fractional point we clicked at
				const real32_t cursorX = Client::cursorPos[0];
				const real32_t realPos = cursorX - position[0];
				const real32_t f = realPos / size[0];

				UpdateValue( f );
				updatingValue = true;
				return true;
			}
			else {
				//FIXME: can we move updatingValue outside this scope?
				updatingValue = false;
				if ( !postExecCommand.empty() ) {
					Command::Append( postExecCommand.c_str() );
					return true;
				}
			}

			return false;
		}

		bool MenuElementSlider::MouseMotionEvent( const struct MouseMotionEvent &ev ) {
			if ( properties.decorative ) {
				return false;
			}

			if ( MouseWithinBounds( Client::cursorPos ) ) {
				tooltip.mouseHovering = true;
				tooltip.lastMousePos = Client::cursorPos;
			}
			else {
				tooltip.mouseHovering = false;
				if ( updatingValue ) {
					const real32_t cursorX = Client::cursorPos[0];
					const real32_t realPos = cursorX - position[0];
					const real32_t f = realPos / size[0];
					UpdateValue( f );
					if ( !postExecCommand.empty() ) {
						Command::Append( postExecCommand.c_str() );
					}
				}
				updatingValue = false;
				return false;
			}

			if ( updatingValue ) {
				// find the fractional point we clicked at
				const real32_t cursorX = Client::cursorPos[0];
				const real32_t realPos = cursorX - position[0];
				const real32_t f = realPos / size[0];

				UpdateValue( f );
			}

			return false;
		}

		bool MenuElementSlider::MouseWheelEvent( const struct MouseWheelEvent &ev ) {
			if ( properties.decorative ) {
				return false;
			}

			return false;
		}

	} // namespace Client

} // namespace XS
