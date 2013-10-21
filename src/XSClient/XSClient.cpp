#include "XSSystem/XSInclude.h"

#include "SDL2/SDL_keycode.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSClient/XSKeys.h"

namespace XS {

	namespace Client {

		void Init( void ) {
			Command::AddCommand( "bind", Cmd_SetBind );
		}

		void NetworkPump( void ) {
			// ...
		}

		void RunFrame( void ) {
			Console::Display();
		}

	} // namespace Client

} // namespace XS
