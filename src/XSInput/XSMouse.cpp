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

			void CaptureMouse( bool doCapture ) {
				captureMouse = doCapture;

				SDL_SetRelativeMouseMode( captureMouse ? SDL_TRUE : SDL_FALSE );
			}

			bool MouseButtonEvent( const struct MouseButtonEvent &ev ) {
				if ( Client::MouseButtonEvent( ev ) ) {
					return true;
				}

				return false;
			}

			bool MouseMotionEvent( const struct MouseMotionEvent &ev ) {
				if ( Client::MouseMotionEvent( ev ) ) {
					return true;
				}

				return false;
			}

			bool MouseWheelEvent( const struct MouseWheelEvent &ev ) {
				if ( Client::MouseWheelEvent( ev ) ) {
					return true;
				}

				return false;
			}

		} // namespace Input

	} // namespace Client

} // namespace XS
