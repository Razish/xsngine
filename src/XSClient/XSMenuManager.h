#pragma once

#include <vector>
#include <unordered_map>

#include "XSClient/XSMenu.h"

namespace XS {

	namespace Renderer {
		struct Material;
	}

	namespace Client {

		class MenuManager {
		private:
			// currently open menus
			std::vector<Menu *>	stack;

			// list of all loaded menus
			std::unordered_map<std::string, Menu *>	menus;

			bool	privateIsOpen;
			vector2	cursorPos; // [0.0, 1.0]

			struct {
				Renderer::Material	*cursor;
			} assets;
			uint32_t cursorWidth, cursorHeight;

		public:
			MenuManager();

			// load a menu from disk
			bool RegisterMenu(
				const char *fileName
			);

			// retrieve the top-most menu
			const Menu *GetCurrentMenu(
				void
			) const;

			// retrieve a menu by name
			const Menu *GetMenuByName(
				const char *menuName
			) const;

			// push a menu onto the stack
			void OpenMenu(
				const char *menuName
			);

			// pop a menu from the stack
			void PopMenu(
				void
			);

			// paint all menus to the screen
			void PaintMenus(
				void
			);

			// draw the cursor on top of all the menus
			void DrawCursor(
				void
			);

			// pass a keyboard event to the top-most menu
			void KeyboardEvent(
				const struct KeyboardEvent &ev
			);

			// pass a mouse motion event to the menu
			void MouseMotionEvent(
				const struct MouseMotionEvent &ev
			);

			// pass a mouse button event to the top-most menu
			void MouseButtonEvent(
				const struct MouseButtonEvent &ev
			);

			const bool	&isOpen;
		};

	} // namespace Client

} // namespace XS
