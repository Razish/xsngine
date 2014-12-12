#pragma once

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSCommand.h"

namespace XS {

	class Console;
	class Cvar;
	class InputField;
	namespace Renderer {
		class View;
		class Font;
	} // namespace Renderer

	namespace Client {

		// driver for global Console instance to handle client interaction/rendering
		extern class ClientConsole {
		private:
			Console			*console;

			bool			 visible;
			int				 scrollAmount;
			unsigned int	 lineCount;// = 24u;
			Cvar 			*con_fontSize;
			InputField		*input;
			Renderer::View	*view;
			Renderer::Font	*font;

		public:
			ClientConsole( Console *console );

			// don't allow default instantiation
			ClientConsole() = delete;
			ClientConsole( const ClientConsole& ) = delete;
			ClientConsole& operator=( const ClientConsole& ) = delete;

			void	Draw	( void );
			bool	KeyEvent( SDL_Keycode key, bool down );
			void	Toggle	( void );
		} *clientConsole;

	} // namespace Client

} // namespace XS
