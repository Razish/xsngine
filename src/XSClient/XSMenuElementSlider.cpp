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

namespace XS {

	namespace Client {

		MenuElementSlider::Assets MenuElementSlider::assets = {};

		void MenuElementSlider::ParseProperties( TokenParser *parser, const char *fileName ) {
			const char *tok = nullptr;
			parser->ParseString( &tok );

			if ( String::CompareCase( tok, "{" ) ) {
				console.Print( PrintLevel::Normal,
					"%s missing opening brace when parsing properties for slider menu element from %s:%i\n",
					XS_FUNCTION,
					fileName,
					parser->GetCurrentLine()
				);
				return;
			}

			while ( true ) {
				parser->ParseString( &tok );

				// see if we reached the end of the slider definition
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

		MenuElementSlider::MenuElementSlider( TokenParser *parser, const char *fileName )
		: cvarName( "" ), postExecCommand( "" ), updatingValue( false )
		{
			std::memset( &properties, 0, sizeof(properties) );
			std::memset( &range, 0, sizeof(range) );

			const char *tok = nullptr;
			parser->ParseString( &tok );

			if ( String::CompareCase( tok, "{" ) ) {
				console.Print( PrintLevel::Normal,
					"%s missing opening brace when parsing slider menu element from %s:%i\n",
					XS_FUNCTION,
					fileName,
					parser->GetCurrentLine()
				);
				return;
			}

			while ( true ) {
				parser->ParseString( &tok );

				// see if we reached the end of the slider definition
				if ( !String::CompareCase( tok, "}" ) ) {
					break;
				}

				// name
				if ( !String::Compare( tok, "name" ) ) {
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
							"%s missing size definition when parsing slider menu element from %s:%i\n",
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

		void MenuElementSlider::Paint( void ) {
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

			const vector2 topLeft(
				position.x * Renderer::state.window.width,
				position.y * Renderer::state.window.height
			);
			const real32_t barWidth = size.x * Renderer::state.window.width;
			const real32_t barHeight = size.y * Renderer::state.window.height;
			const real32_t thumbSize = std::min( barWidth, barHeight );

			Renderer::DrawQuad(
				topLeft.x, topLeft.y,
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
					topLeft.x + (barWidth * f) - (thumbSize / 2.0f), topLeft.y,
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
					topLeft.x + (barWidth * f) - (thumbSize / 2.0f), topLeft.y,
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

			// scale the fractional value up to the real fov value based on the range of the slider
			const real32_t d = range.top - range.bottom;
			const real32_t desiredFov = (frac * d) + range.bottom;

			cvar->Set( desiredFov );
		}

		void MenuElementSlider::MouseButtonEvent( const struct MouseButtonEvent &ev, const vector2 &cursorPos ) {
			Cvar *cvar = Cvar::Get( cvarName );
			if ( !cvar ) {
				return;
			}

			if ( ev.pressed && (ev.button == SDL_BUTTON_LEFT) ) {
				// find the fractional point we clicked at
				const real32_t cursorX = cursorPos.x;
				const real32_t realPos = cursorX - position.x;
				const real32_t f = realPos / size.x;

				UpdateValue( f );
				updatingValue = true;
			}
			else {
				updatingValue = false;
				if ( !postExecCommand.empty() ) {
					Command::Append( postExecCommand.c_str() );
				}
			}
		}

		void MenuElementSlider::MouseMotionEvent( const vector2 &cursorPos ) {
			if ( updatingValue ) {
				// find the fractional point we clicked at
				const real32_t cursorX = cursorPos.x;
				const real32_t realPos = cursorX - position.x;
				const real32_t f = realPos / size.x;

				UpdateValue( f );
			}
		}

	} // namespace Client

} // namespace XS
