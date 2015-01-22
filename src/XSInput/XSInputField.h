#pragma once

#include <vector>
#include <string>

#include <SDL2/SDL_keycode.h>

namespace XS {

	namespace Client {

		class InputField {
		private:
			std::vector<std::string>	history;

			// reverse indexed
			size_t						historyIndex;
			bool						historySeeking;

			std::string					current;
			void						(*callback)( const char *text );
			uint32_t					cursorPos;
			uint32_t					numChars;

			// retrieve the string at the given history index (reverse 1-indexed)
			std::string GetHistory(
				size_t index
			) const;

			// make sure the history index does not result in an overrun/underrun
			void ClampHistory(
				void
			);

		public:
			// don't allow default instantiation
			InputField() = delete;
			InputField( const InputField& ) = delete;
			InputField& operator=( const InputField& ) = delete;

			InputField( void (*callback)( const char *text ) )
			: historyIndex( 0u ), historySeeking( false ), current( "" ), callback( callback ), cursorPos( 0u ),
				numChars( 0u )
			{
			}

			// clear the current input field
			void Clear(
				void
			);

			// pass a key event to the input field (insert, modify)
			bool KeyEvent(
				SDL_Keycode key,
				bool down
			);

			// get a pointer to the current input field string
			const char *GetLine(
				void
			);

			// get the current cursor position
			uint32_t GetCursorPos(
				void
			) const;
		};

	} // namespace Client

} // namespace XS
