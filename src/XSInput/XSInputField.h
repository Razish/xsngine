#pragma once

#include <vector>
#include <string>

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSEvent.h"

namespace XS {

	namespace Client {

		class InputField {

		public:
			using ExecuteFunc = void (*)( const char *text );
			using AutocompleteFunc = const char *(*)( const char *match );

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
			const std::string &GetHistory(
				size_t index
			) const XS_WARN_UNUSED_RESULT;

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
				ExecuteFunc executeFunction,
				AutocompleteFunc autocompleteFunction
			)
			: historyIndex( 0u ), historySeeking( false ), current( "" ), execute( executeFunction ),
				autoComplete( autocompleteFunction ), cursorPos( 0u ), numChars( 0u )
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
			) XS_WARN_UNUSED_RESULT;

			// get the current cursor position
			uint32_t GetCursorPos(
				void
			) const XS_WARN_UNUSED_RESULT;

		};

	} // namespace Client

} // namespace XS
