#pragma once

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSCommand.h"
#include "XSCommon/XSTimer.h"
#include "XSCommon/XSVector.h"

namespace XS {

	namespace Client {

		extern uint64_t frameNum;

		// initialise the Client object, loading related subsystems
		void Init(
			void
		);

		// shut down the client object
		// only called on application quit
		void Shutdown(
			void
		);

		// pump any network events for the ClientGame
		void NetworkPump(
			void
		);

		// run a frame and pass control to the ClientGame
		void RunFrame(
			real64_t dt
		);

		// get the current elapsed time
		// lazily initialised on each unique frame
		real64_t GetElapsedTime(
			TimerResolution resolution = TimerResolution::Milliseconds
		);

		// handle a key event if console is visible
		bool ConsoleKeyEvent(
			SDL_Keycode key,
			bool down
		);

		// handle a mouse motion event if menu is visible
		// returns true if menu caught the event
		bool MouseMotionEvent(
			const struct MouseMotionEvent &ev
		);

		// handle a mouse button event if menu is visible
		// returns true if menu caught the event
		bool MouseButtonEvent(
			const struct MouseButtonEvent &ev
		);

		// handle a keyboard event if menu is visible
		// returns true if menu caught the event
		void KeyboardEvent(
			const struct KeyboardEvent &ev
		);

		// toggle visibility of the console, locking or releasing mouse control
		void Cmd_ToggleConsole(
			const CommandContext * const context
		);

		// draw a client frame and pass control to the ClientGame
		void DrawFrame(
			real64_t frametime
		);

	} // namespace Client

} // namespace XS
