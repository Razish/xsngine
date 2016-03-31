#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSMessageBuffer.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSColours.h"
#include "XSInput/XSInputField.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSClientConsole.h"
#include "XSInput/XSMouse.h"
#include "XSRenderer/XSFont.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Client {

		static void InputCallback( const char *text ) {
			Command::Append( text );
		}

		static const char *InputAutoComplete( const char *match ) {
			//TODO: advanced auto completion (ala zsh/oh-my-zsh)
			return "Autocompleted";
		}

		void ClientConsole::Toggle( void ) {
			if ( visible ) {
				input->Clear();
			}
			visible = !visible;
			Input::CaptureMouse( !visible );
		}

		bool ClientConsole::IsVisible( void ) const {
			return visible;
		}

		// negative for down, positive for up
		void ClientConsole::Scroll( int amount ) {
			if ( amount > 0 ) {
				if ( scrollAmount + amount < static_cast<int32_t>( console->buffer->size() - lineCount ) ) {
					scrollAmount += amount;
				}
			}
			else if ( amount < 0 ) {
				scrollAmount += amount;
				if ( scrollAmount < 0 ) {
					scrollAmount = 0;
				}
			}
		}

		bool ClientConsole::KeyboardEvent( const struct KeyboardEvent &ev ) {
			if ( !visible ) {
				return false;
			}

			if ( ev.down ) {
				if ( ev.key == SDLK_PAGEUP ) {
					Scroll( 1 );
				}
				else if ( ev.key == SDLK_PAGEDOWN ) {
					Scroll( -1 );
				}
				else {
					input->KeyboardEvent( ev );
				}
			}

			return true;
		}

		void ClientConsole::MouseWheelEvent( const struct MouseWheelEvent &ev ) {
			if ( ev.up ) {
				Scroll( ev.amount );
			}
			else {
				Scroll ( ev.amount * -1 );
			}
		}

		ClientConsole::ClientConsole( Console *consoleInstance )
		: console( consoleInstance ), visible( false ), scrollAmount( 0 ), lineCount( 24u ),
			font( nullptr )
		{
			con_fontSize = Cvar::Create( "con_fontSize", "16",
				"Size of the console font", CVAR_ARCHIVE
			);
			input = new InputField( InputCallback, InputAutoComplete );
			view = new Renderer::View( 0u, 0u, true );
			font = Renderer::Font::Register( "console" );
		}

		ClientConsole::~ClientConsole() {
			delete view;
			delete input;
		}

		void ClientConsole::Resize( void ) {
			if ( font ) {
				const uint16_t fontSize = static_cast<uint16_t>( con_fontSize->GetInt32() );
				lineCount = ((view->height / 2) / std::floor( font->lineHeight[fontSize] )) - 1;
			}
		}

		// split a string into multiple chunks
		void ClientConsole::Split( const std::string &line, std::vector<std::string> &lines ) {
			const uint16_t fontSize = static_cast<uint16_t>( con_fontSize->GetInt32() );
			const real32_t lineWidth = view->width;

			ptrdiff_t startOffset = 0u;
			real32_t accumWidth = 0.0f;

			const char *p = line.c_str();
			for ( char c = *p; c != '\0'; c = *p++ ) {
				if ( c == '\t' ) {
					//FIXME: actually handle tab stops properly
					accumWidth += font->GetGlyphWidth( ' ', fontSize );
				}
				else {
					accumWidth += font->GetGlyphWidth( c, fontSize );
				}

				if ( accumWidth >= lineWidth ) {
					// splice and reset counters
					const ptrdiff_t endOffset = p - line.c_str();
					lines.push_back( std::string( line.cbegin() + startOffset, line.cbegin() + endOffset ) );
					startOffset = endOffset;
					accumWidth = 0.0f;
				}
			}

			// push the remainder of the line
			lines.push_back( std::string( line.cbegin() + startOffset, line.cend() ) );
		}

		void ClientConsole::Draw( void ) {
			if ( !visible || !view ) {
				return;
			}

			view->Bind();

			Renderer::DrawQuad(
				0, 0, view->width, view->height / 2,
				0.0f, 0.0f, 1.0f, 1.0f,
				&colourTable[ColourIndex( COLOUR_BLACK )],
				nullptr
			);

			Resize();

			const uint16_t fontSize = static_cast<uint16_t>( con_fontSize->GetInt32() );
			const real32_t x = 0.0f;
			vector2 pos = {
				x,
				(view->height / 2.0f) - font->lineHeight[fontSize]
			};

			// draw the input line
			font->Draw( pos, String::Format( ">%s", input->GetLine() ), fontSize );

			// and now the cursor
			const char *line = input->GetLine();
			real32_t savedX = pos[0];
			pos[0] = font->GetGlyphWidth( '>', fontSize );
			for ( const char *p = line; *p; p++ ) {
				if ( p - line >= static_cast<int32_t>( input->GetCursorPos() ) ) {
					break;
				}
				pos[0] += font->GetGlyphWidth( *p, fontSize );
			}
			//TODO: overstrike mode
			if ( static_cast<uint32_t>( GetElapsedTime() ) & 256 ) {
				// flash every 250ms
				font->Draw( pos, "_", fontSize );
			}
			pos[0] = savedX;

			// draw version information
			static const std::vector<std::string> helpInfoList {
				PRODUCT_NAME " on " OS_STRING " (" ARCH_STRING ")",
				PRODUCT_VERSION,
			};
			const uint16_t helpFontSize = 12u;
			uint32_t numHelpLinesDrawn = 0u;
			for ( auto helpInfo = helpInfoList.crbegin(); helpInfo != helpInfoList.crend(); ++helpInfo ) {
				real32_t helpWidth = 0u;
				for ( const char *p = (*helpInfo).c_str(); *p; p++ ) {
					helpWidth += font->GetGlyphWidth( *p, helpFontSize );
				}
				const vector2 helpPos = {
					view->width - helpWidth,
					(view->height / 2)
						- (font->lineHeight[helpFontSize] * numHelpLinesDrawn)
						- font->lineHeight[fontSize]
						- 4u
				};
				font->Draw( helpPos, *helpInfo, helpFontSize );
				numHelpLinesDrawn++;
			}

			// grab a subset of the console buffer that we may want to draw - we do word-wrapping in realtime
			// line breaks and carriage returns are preprocessed in the Console
			const uint32_t numLines = console->buffer->size();
			const uint32_t start = std::max(
				0,
				static_cast<int32_t>( numLines )
					- static_cast<int32_t>( scrollAmount )
					- static_cast<int32_t>( lineCount )
			);
			const uint32_t begin = std::min( numLines, start );
			const uint32_t end = std::min( begin + lineCount, numLines );
			std::vector<std::string> lines( console->buffer->begin() + begin, console->buffer->begin() + end );

			uint32_t drawn = 0u;
			for ( auto line = lines.crbegin(); line != lines.crend(); ++line ) {
				// substring of renderable characters
				std::vector<std::string> subsets;
				Split( *line, subsets );
				for ( auto subset = subsets.crbegin(); subset != subsets.crend(); ++subset ) {
					const uint32_t subsetLineCount = font->GetTextLineCount( pos, *subset, fontSize );
					drawn += subsetLineCount;
					if ( drawn > lineCount ) {
						break;
					}
					pos[1] -= font->lineHeight[fontSize] * subsetLineCount;
					font->Draw( pos, *subset, fontSize, &colourTable[ColourIndex( COLOUR_WHITE )] );
				}
			}
		}

	} // namespace Client

} // namespace XS
