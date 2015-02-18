#pragma once

#include <unordered_map>
#include <string>

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSCommand.h"
#include "XSCommon/XSEvent.h"

namespace XS {

	namespace Client {

		extern std::unordered_map<SDL_Keycode, bool> keystate;

		// return an ASCII character for the given SDL keycode
		char GetPrintableCharForKeycode(
			SDL_Keycode keycode
		);

		// pass input events to the client system (console -> menus -> binds/ingame)
		void KeyboardEvent(
			const KeyboardEvent &ev
		);
		void MouseWheelEvent(
			const MouseWheelEvent &ev
		);
		void MouseButtonEvent(
			const MouseButtonEvent &ev
		);
		void MouseMotionEvent(
			const MouseMotionEvent &ev
		);

		// list all key bindings
		void Cmd_ListBinds(
			const CommandContext * const context
		);

		// set a key binding
		void Cmd_SetBind(
			const CommandContext * const context
		);

		// append all key bindings to the specified string
		void WriteBinds(
			std::string &str
		);

	} // namespace Client

} // namespace XS
