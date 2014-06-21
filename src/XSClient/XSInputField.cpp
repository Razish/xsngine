#include "XSCommon/XSCommon.h"
#include "XSClient/XSInputField.h"
#include "XSClient/XSKeys.h"

namespace XS {

	namespace Client {

		const char *InputField::GetLine( void ) {
			return current.c_str();
		}

		void InputField::Clear( void ) {
			current.clear();
		}

		bool InputField::KeyEvent( SDL_Keycode key, bool down ) {
			if ( char c = GetPrintableCharForKeycode( key ) ) {
				// printable chars
				current += c;
				return true;
			}
			else if ( key == SDLK_RETURN ) {
				// commit text
				if ( callback )
					callback( current.c_str() );

				if ( !current.empty() )
					history.push_back( current.c_str() );

				Clear();
				return true;
			}

			return false;
		}

	} // namespace Client

} // namespace XS
