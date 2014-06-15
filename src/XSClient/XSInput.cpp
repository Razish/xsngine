#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSEvent.h"
#include "XSClient/XSInput.h"
#include "XSClient/XSKeys.h"

namespace XS {

	namespace Client {

		// Client input instance. Access via Client::input.Blah()
		Input input;

		// constructor
		Input::Input() {
			// raw input
			SDL_SetRelativeMouseMode( SDL_TRUE );
		}

		void Input::Poll( void ) const {
			SDL_Event e;

			while ( SDL_PollEvent( &e ) ) {
				switch ( e.type ) {
				case SDL_QUIT:
					throw( XSError( "Quit application" ) );
					break;

				case SDL_KEYDOWN:
					{
						SDL_Keycode key = e.key.keysym.sym;

						keystate[key] = true;

						XSEvent ev;
						ev.keyEvent.key = key;
						ev.keyEvent.down = true;

						Event::Queue( EventType::KEY, &ev );
					}
					break;

				case SDL_KEYUP:
					{
						SDL_Keycode key = e.key.keysym.sym;

						keystate[key] = false;

						XSEvent ev;
						ev.keyEvent.key = key;
						ev.keyEvent.down = false;

						Event::Queue( EventType::KEY, &ev );
					}
					break;

				default:
				//	throw( XSError( String::Format( "Unhandled SDL event %d", e.type ) ) );
					break;
				}
			}

			//TODO: movement command generation?
		}

	} // namespace Client

} // namespace XS
