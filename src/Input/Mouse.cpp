#include <queue>

#include <SDL2/SDL.h>

#include "Common/Common.h"
#include "Common/String.h"
#include "Common/Event.h"
#include "Common/Vector.h"
#include "Common/Cvar.h"
#include "Common/Console.h"
#include "Client/Client.h"
#include "Client/ClientConsole.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"

//#define MOUSE_SMOOTHING

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
