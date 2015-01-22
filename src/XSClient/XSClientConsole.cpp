#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSMessageBuffer.h"
#include "XSCommon/XSString.h"
#include "XSInput/XSInputField.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSClientConsole.h"
#include "XSRenderer/XSFont.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Client {

		static void InputCallback( const char *text ) {
			Command::Append( text );
		}

		void ClientConsole::Toggle( void ) {
			if ( visible ) {
				input->Clear();
			}
			visible = !visible;
			SDL_SetRelativeMouseMode( visible ? SDL_FALSE : SDL_TRUE );
		}

		bool ClientConsole::KeyEvent( SDL_Keycode key, bool down ) {
			if ( !visible ) {
				return false;
			}

			if ( down ) {
				//TODO: scroll input cursor with SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT
				//TODO: scroll up/down through history with SDL_SCANCODE_UP, SDL_SCANCODE_DOWN
				if ( key == SDLK_BACKQUOTE ) {
					// hardcoded console exit
					Toggle();
					return true;
				}
				else if ( key == SDLK_PAGEUP ) {
					if ( scrollAmount + 1 < console->buffer->GetNumLines() - lineCount ) {
						scrollAmount++;
					}
					return true;
				}
				else if ( key == SDLK_PAGEDOWN ) {
					scrollAmount--;
					if ( scrollAmount < 0 ) {
						scrollAmount = 0;
					}
					return true;
				}
				else if ( input->KeyEvent( key, down ) ) {
					return true;
				}
			}

			return false;
		}

		ClientConsole::ClientConsole( Console *consoleInstance )
		: console( consoleInstance ), visible( false ), scrollAmount( 0 ), lineCount( 24u ), font( nullptr )
		{
			const uint32_t width = Cvar::Get( "vid_width" )->GetInt();
			const uint32_t height = Cvar::Get( "vid_height" )->GetInt();

			con_fontSize = Cvar::Create( "con_fontSize", "12", "Size of the console font", CVAR_ARCHIVE );
			input = new InputField( InputCallback );
			view = new Renderer::View( width, height, true );
		}

		void ClientConsole::Resize( void ) {
			if ( font ) {
				const uint32_t height = Cvar::Get( "vid_height" )->GetInt();
				lineCount = ((height / 2) / std::floor( font->lineHeight )) - 1;
			}
		}

		void ClientConsole::Draw( void ) {
			if ( !visible ) {
				return;
			}

			view->Bind();

			// have to register it each frame so we can change the font size at runtime
			font = Renderer::Font::Register( "console", static_cast<uint16_t>( con_fontSize->GetInt() ) );

			const uint32_t width = Cvar::Get( "vid_width" )->GetInt();
			const uint32_t height = Cvar::Get( "vid_height" )->GetInt();
			static const vector4 colour( 1.0f, 0.0f, 0.0f, 1.0f );
			Renderer::DrawQuad( 0, 0, width, height / 2, 0.0f, 0.0f, 1.0f, 1.0f, &colour, nullptr );

			Resize();

			// draw the console text
			const uint32_t numLines = console->buffer->GetNumLines();
			const uint32_t start = std::max(
				0,
				static_cast<int32_t>( numLines ) - scrollAmount - static_cast<int32_t>( lineCount )
			);
			std::vector<std::string> lines = console->buffer->FetchLines( start, lineCount );

			// TODO: might have to draw lines in reverse to compensate for one buffer element spanning multiple lines
			const float x = 0.0f;
			vector2 pos( x, 0.0f );
			uint32_t drawn = 0u;
			for ( const auto &it : lines ) {
				const uint32_t linesToDraw = font->GetTextLineCount( pos, it );
				drawn += linesToDraw;
				if ( drawn > lineCount ) {
					break;
				}
				font->Draw( pos, it );
				pos.y += linesToDraw * font->lineHeight;
			}

			// draw the input line
			font->Draw( pos, String::Format( ">%s", input->GetLine() ) );

			// and now the cursor
			const char *line = input->GetLine();
			pos.x = font->GetGlyphWidth( '>' );
			for ( const char *p = line; *p; p++ ) {
				if ( p - line >= input->GetCursorPos() ) {
					break;
				}
				pos.x += font->GetGlyphWidth( *p );
			}
			//TODO: overstrike mode
			if ( static_cast<uint32_t>( GetElapsedTime() ) & 256 ) {
				// flash every 500ms
				font->Draw( pos, "_" );
			}

			// adjust for next line
			pos.x = x;
			pos.y += font->lineHeight;
		}

	} // namespace Client

} // namespace XS
