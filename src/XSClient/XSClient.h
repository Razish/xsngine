#pragma once

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSCommand.h"
#include "XSCommon/XSTimer.h"
#include "XSCommon/XSVector.h"

namespace XS {

	namespace Client {

		extern uint64_t frameNum;

		extern struct ClientState {

			vector3 viewAngles;
		} state;

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
			real64_t dt
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
