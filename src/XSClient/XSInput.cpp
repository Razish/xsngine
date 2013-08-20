#include "XSClient/XSInput.h"
#include "XSCommon/XSFormat.h"

#include <SDL2/SDL_events.h>

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
					//TODO: key binds
					if ( e.key.keysym.sym == SDLK_ESCAPE ) {
						throw( "User quit" );
						break;
					}
					break;

				default:
				//	throw( Format( "Unhandled SDL event %d", e.type ) );
					break;
				}
			}
		}

	}; // namespace Client

}; // namespace XS
