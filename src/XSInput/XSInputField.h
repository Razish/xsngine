#pragma once

#include <vector>
#include <string>

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSEvent.h"

namespace XS {

	namespace Client {

		class InputField {
		private:
			std::vector<std::string>	 history;

			// reverse indexed
			size_t						 historyIndex;
			bool						 historySeeking;

			std::string					 current;
			void						 (*execute)( const char *text );
			const char					*(*autoComplete)( const char *match );
			uint32_t					 cursorPos;
			uint32_t					 numChars;

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

			InputField(
				void (*executeFunc)( const char *text ),
				const char *(*autoCompleteFunc)( const char *match )
			)
			: historyIndex( 0u ), historySeeking( false ), current( "" ), execute( executeFunc ),
				autoComplete( autoCompleteFunc ), cursorPos( 0u ), numChars( 0u )
			{
			}

			// clear the current input field
			void Clear(
				void
			);

			// pass a key event to the input field (insert, modify)
			void KeyboardEvent(
				const KeyboardEvent &ev
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
