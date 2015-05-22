#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSTokenParser.h"
#include "XSCommon/XSEvent.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSMenu.h"
#include "XSClient/XSMenuElement.h"
#include "XSClient/XSMenuElementButton.h"
#include "XSClient/XSMenuElementSlider.h"
#include "XSClient/XSMenuElementText.h"

namespace XS {

	namespace Client {

		uint32_t Menu::version = 3u;

		Menu::Menu( const char *fileName ) {
			const File f( fileName );
			if ( !f.open ) {
				return;
			}

			console.Print( PrintLevel::Developer,
				"Loading menu \"%s\"\n",
				fileName
			);
			Indent indent( 1 );

			char *buffer = new char[f.length];
				f.Read( reinterpret_cast<uint8_t *>( buffer ) );
				TokenParser *parser = new TokenParser( buffer );

				// check the version identifier
				const char *token = parser->ParseToken();
				if ( !String::Compare( token, "version" ) ) {
					uint32_t ui = 0u;
					parser->ParseUInt32( &ui );
					if ( ui != Menu::version ) {
						console.Print( PrintLevel::Normal,
							"%s invalid xmenu file '%s' wrong version identifier (%u != %u)\n",
							XS_FUNCTION,
							fileName,
							ui,
							Menu::version
						);
						return;
					}
				}
				else {
					console.Print( PrintLevel::Normal,
						"%s invalid xmenu file '%s' missing version identifier!\n",
						XS_FUNCTION,
						fileName
					);
					return;
				}

				token = parser->ParseToken();
				name = token;

				// should be a valid menu, try parsing it out
				MenuElement *element = nullptr;
				while ( true ) {
					const char *token = parser->ParseToken();
					if ( !token[0] || !String::Compare( token, "}" ) ) {
						// end of file
						element = nullptr;
						break;
					}
					else if ( !String::CompareCase( token, "#" ) ) {
						// # is used as a comment
						parser->SkipLine();
					}
					// position
					else if ( !String::Compare( token, "position" ) ) {
						real32_t x, y;
						if ( parser->ParseReal32( &x ) || parser->ParseReal32( &y ) ) {
							console.Print( PrintLevel::Normal,
								"%s missing position definition when parsing menu from %s:%i\n",
								XS_FUNCTION,
								fileName,
								parser->GetCurrentLine()
							);
							parser->SkipLine();
						}
						else {
							position.x = x;
							position.y = y;
						}
					}
					// size
					else if ( !String::Compare( token, "size" ) ) {
						real32_t w, h;
						if ( parser->ParseReal32( &w ) || parser->ParseReal32( &h ) ) {
							console.Print( PrintLevel::Normal,
								"%s missing size definition when parsing menu from %s:%i\n",
								XS_FUNCTION,
								fileName,
								parser->GetCurrentLine()
							);
							parser->SkipLine();
						}
						else {
							size.w = w;
							size.h = h;
						}
					}
					else if ( !String::Compare( token, "button" ) ) {
						if ( element ) {
							console.Print( PrintLevel::Normal,
								"%s tried to parse button menu element in %s:%i whilst parsing another element\n",
								XS_FUNCTION,
								parser->GetCurrentLine(),
								fileName
							);
							break;
						}

						element = new MenuElementButton( parser, fileName );
						element->parent = this;
						elements.push_back( element );
						element = nullptr;

						parser->SkipLine();
					}
					else if ( !String::Compare( token, "slider" ) ) {
						if ( element ) {
							console.Print( PrintLevel::Normal,
								"%s tried to parse slider menu element in %s:%i whilst parsing another element\n",
								XS_FUNCTION,
								parser->GetCurrentLine(),
								fileName
							);
							break;
						}

						element = new MenuElementSlider( parser, fileName );
						element->parent = this;
						elements.push_back( element );
						element = nullptr;

						parser->SkipLine();
					}
					else if ( !String::Compare( token, "text" ) ) {
						if ( element ) {
							console.Print( PrintLevel::Normal,
								"%s tried to parse text menu element in %s:%i whilst parsing another element\n",
								XS_FUNCTION,
								parser->GetCurrentLine(),
								fileName
							);
							break;
						}

						element = new MenuElementText( parser, fileName );
						element->parent = this;
						elements.push_back( element );
						element = nullptr;

						parser->SkipLine();
					}
					else {
						parser->SkipLine();
					}
				}

				delete parser;

			delete[] buffer;
		}

		void Menu::Paint( void ) {
			for ( auto element : elements ) {
				if ( !element->properties.hidden ) {
					element->Paint();
				}
			}
		}

		bool Menu::MouseButtonEvent( const struct MouseButtonEvent &ev, const vector2 &cursorPos ) {
			const real32_t cursorX = cursorPos.x;
			const real32_t cursorY = cursorPos.y;
			const real32_t menuX = position.x;
			const real32_t menuY = position.y;
			const real32_t menuW = size.x;
			const real32_t menuH = size.y;
			if ( cursorX > menuX && cursorX < (menuX + menuW)
				&& cursorY > menuY && cursorY < (menuY + menuH) )
			{
				// pass it down the chain
				for ( auto element : elements ) {
					if ( !element->properties.decorative ) {
						element->MouseButtonEvent( ev, cursorPos );
					}
				}
				return true;
			}
			else {
				return false;
			}
		}

		bool Menu::MouseMotionEvent( const vector2 &cursorPos ) {
			const real32_t cursorX = cursorPos.x;
			const real32_t cursorY = cursorPos.y;
			const real32_t menuX = position.x;
			const real32_t menuY = position.y;
			const real32_t menuW = size.x;
			const real32_t menuH = size.y;
			if ( cursorX > menuX && cursorX < (menuX + menuW)
				&& cursorY > menuY && cursorY < (menuY + menuH) )
			{
				// pass it down the chain
				for ( auto element : elements ) {
					if ( !element->properties.decorative ) {
						element->MouseMotionEvent( cursorPos );
					}
				}
				return true;
			}
			return false;
		}

	} // namespace Client

} // namespace XS
