#pragma once

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSCommand.h"
#include "XSClient/XSClientConsole.h"

namespace XS {

	namespace Client {

		extern ClientConsole *clientConsole;

		void	Init				( void );
		void	Shutdown			( void );
		void	NetworkPump			( void );
		void	RunFrame			( double dt );
		bool	ConsoleKeyEvent		( SDL_Keycode key, bool down );
		void	Cmd_ToggleConsole	( const commandContext_t * const context );
		void	DrawFrame			( double frametime );

	} // namespace Client

} // namespace XS
