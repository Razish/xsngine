#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSTokenParser.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSMenuElement.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSFont.h"

namespace XS {

	namespace Client {

		MenuElement::MenuElement() {
			std::memset( &properties, 0, sizeof(properties) );
			std::memset( &tooltip, 0, sizeof(tooltip) );
		}

		MenuElement::~MenuElement() {
			// ...
		}

		void MenuElement::ParseTooltip( TokenParser *parser, const char *fileName ) {
			const char *tok = nullptr;
			parser->ParseString( &tok );

			if ( String::CompareCase( tok, "{" ) ) {
				//TODO: show which element the tooltip belongs to
				//	...which we may not have parsed out yet
				console.Print( PrintLevel::Normal,
					"%s missing opening brace when parsing tooltip from %s:%i\n",
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

				// font + size
				else if ( !String::Compare( tok, "font" ) ) {
					if ( parser->ParseString( &tok ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing font definition when parsing cvar menu element from %s:%i\n",
							XS_FUNCTION,
							fileName,
							parser->GetCurrentLine()
						);
						parser->SkipLine();
						continue;
					}
					else {
						tooltip.font = Renderer::Font::Register( tok );
					}

					uint32_t tmpPointSize;
					if ( parser->ParseUInt32( &tmpPointSize ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing font size definition when parsing cvar menu element from %s:%i\n",
							XS_FUNCTION,
							fileName,
							parser->GetCurrentLine()
						);
					}
					else {
						tooltip.pointSize = static_cast<uint16_t>( tmpPointSize );
					}

					parser->SkipLine();
				}

				else if ( !String::Compare( tok, "text" ) ) {
					if ( parser->ParseString( &tok ) ) {
						console.Print( PrintLevel::Normal,
							"%s missing tooltip definition when parsing slider menu element from %s:%i\n",
							XS_FUNCTION,
							fileName,
							parser->GetCurrentLine()
						);
					}
					else {
						tooltip.text = tok;
					}
					parser->SkipLine();
				}
			}
		}

		void MenuElement::DrawTooltip( void ) {
			const real64_t currentTime = GetElapsedTime();
			if ( tooltip.mouseHovering ) {
				tooltip.lastMouseTime = currentTime;
			}
			const real64_t fadeTime = 333.0;
			if ( /*!updatingValue &&*/ tooltip.lastMouseTime > currentTime - fadeTime ) {
				const real64_t alpha = 1.0 - ((currentTime - tooltip.lastMouseTime) / fadeTime);
				static vector4 colour( 1.0f, 1.0f, 1.0f, 1.0f );
				colour.a = alpha;
				const vector2 pos(
					(tooltip.lastMousePos.x + 0.03333f) * Renderer::state.window.width,
					tooltip.lastMousePos.y * Renderer::state.window.height
				);
				tooltip.font->Draw(
					pos,
					tooltip.text,
					tooltip.pointSize,
					&colour
				);
			}
		}

	} // namespace Client

} // namespace XS
