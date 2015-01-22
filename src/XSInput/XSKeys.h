#pragma once

#include <unordered_map>
#include <string>

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSCommand.h"

namespace XS {

	namespace Client {

		extern std::unordered_map<SDL_Keycode, bool>	keystate;

		// return an ASCII character for the given SDL keycode
		char GetPrintableCharForKeycode(
			SDL_Keycode keycode
		);

		// pass a key event to the Client system (binds, console, ingame movement)
		void KeyEvent(
			SDL_Keycode key,
			bool down
		);

		// list all key bindings
		void Cmd_ListBinds(
			const commandContext_t * const context
		);

		// set a key binding
		void Cmd_SetBind(
			const commandContext_t * const context
		);

		// append all key bindings to the specified string
		void WriteBinds(
			std::string &str
		);

	} // namespace Client

} // namespace XS
