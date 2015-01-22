#pragma once

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSCommand.h"
#include "XSCommon/XSTimer.h"

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
			double dt
		);

		// get the current elapsed time
		// lazily initialised on each unique frame
		double GetElapsedTime(
			Timer::Resolution resolution = Timer::Resolution::MILLISECONDS
		);

		// handle a key event if console is visible
		bool ConsoleKeyEvent(
			SDL_Keycode key,
			bool down
		);

		// toggle visibility of the console, locking or releasing mouse control
		void Cmd_ToggleConsole(
			const commandContext_t * const context
		);

		// draw a client frame and pass control to the ClientGame
		void DrawFrame(
			double frametime
		);

	} // namespace Client

} // namespace XS
