#pragma once

#include <SDL2/SDL_keycode.h>

#include "Common/Command.h"
#include "Common/Event.h"

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
		Console			*console;
		bool			 privateIsVisible;
		int32_t			 scrollAmount;
		uint32_t		 lineCount;
		Cvar 			*con_fontSize = nullptr;
		InputField		*input = nullptr;
		Renderer::View	*view = nullptr;
		Renderer::Font	*font;

		void Scroll(
			int amount // negative for down, positive for up
		);

		void Split(
			const std::string &line,
			std::vector<std::string> &lines
		);

	public:
		const bool	&isVisible;

		// don't allow default instantiation
		ClientConsole() = delete;
		ClientConsole( const ClientConsole& ) = delete;
		ClientConsole& operator=( const ClientConsole& ) = delete;

		// instantiate the ClientConsole for a specified Console object
		ClientConsole(
			Console *console
		);
		~ClientConsole();

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
		) XS_WARN_UNUSED_RESULT;

		// handle a mouse wheel event for scrolling
		bool MouseButtonEvent(
			const struct MouseButtonEvent &ev
		) XS_WARN_UNUSED_RESULT;

		// handle a mouse wheel event for scrolling
		bool MouseMotionEvent(
			const struct MouseMotionEvent &ev
		) XS_WARN_UNUSED_RESULT;

		// handle a mouse wheel event for scrolling
		bool MouseWheelEvent(
			const struct MouseWheelEvent &ev
		) XS_WARN_UNUSED_RESULT;

		// toggle visibility of the console
		void Toggle(
			void
		);

	} *clientConsole;

} // namespace Client
