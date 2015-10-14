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
				if ( scrollAmount + amount < static_cast<int32_t>( console->buffer->GetNumLines() - lineCount ) ) {
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
		: console( consoleInstance ), visible( false ), scrollAmount( 0 ), lineCount( 24u ), font( nullptr )
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
				lineCount = ((Renderer::rdState.window.height / 2) / std::floor( font->lineHeight[fontSize] )) - 1;
			}
		}

		void ClientConsole::Draw( void ) {
			if ( !visible || !view ) {
				return;
			}

			view->Bind();

			Renderer::DrawQuad(
				0, 0, Renderer::rdState.window.width, Renderer::rdState.window.height / 2,
				0.0f, 0.0f, 1.0f, 1.0f,
				&colourTable[ColourIndex( COLOUR_BLACK )],
				nullptr
			);

			Resize();

			// draw the console text
			const uint32_t numLines = console->buffer->GetNumLines();
			const uint32_t start = std::max(
				0,
				static_cast<int32_t>( numLines ) - static_cast<int32_t>( scrollAmount )
					- static_cast<int32_t>( lineCount )
			);
			std::vector<std::string> lines = console->buffer->FetchLines( start, lineCount );

			// TODO: might have to draw lines in reverse to compensate for one buffer element spanning multiple
			//	lines
			const real32_t x = 0.0f;
			vector2 pos = { x, 0.0f };
			uint32_t drawn = 0u;
			const uint16_t fontSize = static_cast<uint16_t>( con_fontSize->GetInt32() );
			for ( const auto &it : lines ) {
				const uint32_t linesToDraw = font->GetTextLineCount( pos, it, fontSize );
				drawn += linesToDraw;
				if ( drawn > lineCount ) {
					break;
				}
				font->Draw( pos, it, fontSize, &colourTable[ColourIndex( COLOUR_WHITE )] );
				pos[1] += linesToDraw * font->lineHeight[fontSize];
			}

			// draw the input line
			font->Draw( pos, String::Format( ">%s", input->GetLine() ), fontSize );

			// and now the cursor
			const char *line = input->GetLine();
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

			// draw version information
			const char *versionText = WINDOW_TITLE;
			real32_t versionWidth = 2.0f;
			for ( const char *p = versionText; *p; p++ ) {
				versionWidth += font->GetGlyphWidth( *p, 12u );
			}
			vector2 versionPos = {
				Renderer::rdState.window.width - versionWidth,
				(Renderer::rdState.window.height / 2) - font->lineHeight[12u] - 4u
			};
			font->Draw( versionPos, versionText, 12u );

			// adjust for next line
			pos[0] = x;
			pos[1] += font->lineHeight[fontSize];
		}

	} // namespace Client

} // namespace XS
