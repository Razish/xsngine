#include <queue>

#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSConsole.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSClientConsole.h"
#include "XSInput/XSInput.h"
#include "XSRenderer/XSRenderer.h"

//#define MOUSE_SMOOTHING

namespace XS {

	namespace Client {

		// input events will accumulate throughout the frame, and the final mouse movement (i.e. affecting viewangles)
		//	will be calculated when a movement command is generated (taking into account smoothing, acceleration,
		//	sensitivity)

		namespace Input {

			static bool captureMouse = false;

			void CaptureMouse( bool capture ) {
				captureMouse = capture;

				SDL_SetRelativeMouseMode( captureMouse ? SDL_TRUE : SDL_FALSE );
			}

			void MouseWheelEvent( const struct MouseWheelEvent &ev ) {
				if ( clientConsole ) {
					clientConsole->MouseWheelEvent( ev );
				}
			}

			void MouseButtonEvent( const struct MouseButtonEvent &ev ) {
				if ( Client::MouseButtonEvent( ev ) ) {
					return;
				}
			}

			// called directly from input polling
			void MouseMotionEvent( const struct MouseMotionEvent &ev ) {
				// see if the client (menu) handled the mouse movement
				if ( Client::MouseMotionEvent( ev ) ) {
					return;
				}
			}

		} // namespace Input

	} // namespace Client

} // namespace XS
