#pragma once

#include <unordered_map>

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSCommand.h"

namespace XS {

	namespace Client {

		extern std::unordered_map<SDL_Keycode, bool> keystate;

		void KeyEvent		( SDL_Keycode key, bool down );
		void Cmd_ListBinds	( const commandContext_t * const context );
		void Cmd_SetBind	( const commandContext_t * const context );
		void WriteBinds		( std::string &str );

	} // namespace Client

} // namespace XS
