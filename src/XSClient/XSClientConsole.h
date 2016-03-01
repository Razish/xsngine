#pragma once

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSCommand.h"
#include "XSCommon/XSEvent.h"

namespace XS {

	// forward declaration
	class Console;
	class Cvar;
	class InputField;
	namespace Renderer {
		struct View;
		class Font;
	} // namespace Renderer

	namespace Client {

		// driver for global Console instance to handle client interaction/rendering
		extern class ClientConsole {

		private:
			Console			*console = nullptr;
			bool			 visible = false;
			int32_t			 scrollAmount = 0;
			uint32_t		 lineCount = 0u;
			Cvar 			*con_fontSize = nullptr;
			InputField		*input = nullptr;
			Renderer::View	*view = nullptr;
			Renderer::Font	*font = nullptr;

			void Scroll(
				int amount // negative for down, positive for up
			);

		public:
			// instantiate the ClientConsole for a specified Console object
			ClientConsole(
				Console *console
			);
			~ClientConsole();

			// don't allow default instantiation
			ClientConsole() = delete;
			ClientConsole( const ClientConsole& ) = delete;
			ClientConsole& operator=( const ClientConsole& ) = delete;

			// draw the console background, text and input field
			void Draw(
				void
			);

			// calculate the correct number of lines for the window height and font line height
			void Resize(
				void
			);

			// handle a key event for the input field
			bool KeyboardEvent(
				const struct KeyboardEvent &ev
			);

			// handle a mouse wheel event for scrolling
			void MouseWheelEvent(
				const struct MouseWheelEvent &ev
			);

			// toggle visibility of the console
			void Toggle(
				void
			);

			// whether or not the console is visible
			bool IsVisible(
				void
			) const XS_WARN_UNUSED_RESULT;

		} *clientConsole;

	} // namespace Client

} // namespace XS
