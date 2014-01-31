#pragma once

namespace XS {

	namespace Client {

		extern std::unordered_map<SDL_Keycode, bool> keystate;

		void KeyEvent			( SDL_Keycode key, bool down );
		void Cmd_SetBind		( const commandContext_t *context );
		void WriteBinds			( std::string &str );

	} // namespace Client

} // namespace XS
