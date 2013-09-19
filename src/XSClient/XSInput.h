#pragma once

#include <map>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include "XSCommon/XSConsole.h"

namespace XS {

	namespace Client {

		// exists in XSClient/XSInput.cpp for now
		extern class Input {
		private:
			std::map<SDL_Keycode, bool> keystate;
			std::map<SDL_Keycode, std::string> bind;

		public:
			Input() {
				// raw input
				SDL_SetRelativeMouseMode( SDL_TRUE );
			}
			void Poll( void );
		} input;

	}; // namespace Client

}; // namespace XS
