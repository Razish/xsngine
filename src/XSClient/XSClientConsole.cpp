#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSMessageBuffer.h"
#include "XSInput/XSInputField.h"
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

		void ClientConsole::Draw( void ) {
			if ( !visible ) {
				return;
			}

			view->Bind();

			// have to register it each frame so we can change the font size at runtime
			font = Renderer::Font::Register( "console", static_cast<uint16_t>( con_fontSize->GetInt() ) );

			const uint32_t width = Cvar::Get( "vid_width" )->GetInt();
			const uint32_t height = Cvar::Get( "vid_height" )->GetInt();
			static const vector4 colour( 1.0f, 0.0f, 0.0f, 0.25f );
			Renderer::DrawQuad( 0, 0, width, height / 2, 0.0f, 0.0f, 1.0f, 1.0f, &colour, nullptr );

			std::vector<std::string> lines = console->buffer->GetLines( lineCount );
			vector2 pos( 0.0f, 0.0f );
			for ( const auto &it : lines ) {
				const int linesDrawn = font->Draw( pos, it );
				pos.y += linesDrawn * font->lineHeight;
			}
		}

	} // namespace Client

} // namespace XS
