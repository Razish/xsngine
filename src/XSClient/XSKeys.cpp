#include "XSSystem/XSInclude.h"

#include "SDL2/SDL_keycode.h"

#include "XSClient/XSKeys.h"
#include "XSCommon/XSConsole.h"

namespace XS {
	
	namespace Client {
		
		void KeyEvent( SDL_Keycode key, bool down ) {
			Console::Print( "KeyEvent: key: %i, down: %i\n", key, down );
		}

	} // namespace Client

} // namespace XS
