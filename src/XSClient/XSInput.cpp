#include "XSSystem/XSInclude.h"

#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keycode.h"

#include "XSClient/XSInput.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSEvent.h"

namespace XS {

	namespace Client {

		// Client input instance. Access via Client::input.Blah()
		Input input;

		// constructor
		Input::Input() {
			// raw input
			SDL_SetRelativeMouseMode( SDL_TRUE );
		}

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

						Event::XSEvent ev;
						ev.keyEvent.key = key;
						ev.keyEvent.down = true;

						Event::Queue( Event::KEYEVENT, ev );
					}
					break;

				case SDL_KEYUP:
					{
						SDL_Keycode key = e.key.keysym.sym;

						keystate[key] = false;

						Event::XSEvent ev;
						ev.keyEvent.key = key;
						ev.keyEvent.down = false;

						Event::Queue( Event::KEYEVENT, ev );
					}
					break;

				default:
				//	throw( String::Format( "Unhandled SDL event %d", e.type ) );
					break;
				}
			}
		}

	} // namespace Client

} // namespace XS
