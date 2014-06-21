#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL_keycode.h>

namespace XS {

	namespace Client {

		class InputField {
		private:
			std::vector<std::string> history;
			std::string current;
			void (*callback)( const char *text );

		public:
			// don't allow default instantiation
			InputField() = delete;
			InputField( const InputField& ) = delete;
			InputField& operator=( const InputField& ) = delete;

			InputField( void (*callback)( const char *text ) ) : callback( callback ) {}

			// clear the current line
			void Clear( void );
			bool KeyEvent( SDL_Keycode key, bool down );
			const char *GetLine( void );
		};

	} // namespace Client

} // namespace XS
