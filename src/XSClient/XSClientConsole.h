#pragma once

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSCvar.h"
#include "XSInput/XSInputField.h"
#include "XSRenderer/XSFont.h"
#include "XSRenderer/XSView.h"

namespace XS {

	namespace Client {

		// driver for global Console instance to handle client interaction/rendering
		class ClientConsole {
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
		};

	} // namespace Client

} // namespace XS
