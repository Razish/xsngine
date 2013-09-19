#include "XSClient/XSInput.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSEvent.h"

namespace XS {

	namespace Client {

		// Client input instance. Access via Client::input.Blah()
		Input input;

		void Input::Poll( void ) {
			SDL_Event e;

			while ( SDL_PollEvent( &e ) ) {
				switch ( e.type ) {
				case SDL_QUIT:
					throw( "Quit application" );
					break;

				case SDL_KEYDOWN:
					{
						SDL_Keycode key = e.key.keysym.sym;

						keystate[key] = true;

						Common::XSEvent ev;
						ev.data.keyEvent.key = key;
						ev.data.keyEvent.down = true;

						Common::QueueEvent( Common::KEYEVENT, ev );
					//	Command::buffer.Append( bind[key] );
					}
					break;

				case SDL_KEYUP:
					{
						SDL_Keycode key = e.key.keysym.sym;

						keystate[key] = false;

						Common::XSEvent ev;
						ev.data.keyEvent.key = key;
						ev.data.keyEvent.down = false;

						Common::QueueEvent( Common::KEYEVENT, ev );
					}
					break;

				default:
				//	throw( String::Format( "Unhandled SDL event %d", e.type ).c_str() );
					break;
				}
			}
		}

	}; // namespace Client

}; // namespace XS
