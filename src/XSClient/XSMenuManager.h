#pragma once

#include <vector>
#include <unordered_map>

#include "XSClient/XSMenu.h"
#include "XSRenderer/XSView.h"

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

			// view the menus will be rendered to
			const Renderer::View	&view;

			bool	privateIsOpen = false;

			struct {
				Renderer::Material	*cursor = nullptr;
			} assets;
			uint32_t cursorWidth = 0u;
			uint32_t cursorHeight = 0u;

		public:
			// don't allow default instantiation
			MenuManager() = delete;
			MenuManager( const MenuManager& ) = delete;
			MenuManager& operator=( const MenuManager& ) = delete;

			// instantiate a MenuManager
			MenuManager(
				const Renderer::View &view
			);
			~MenuManager();

			// load a menu from disk
			bool RegisterMenu(
				const char *fileName
			);

			// retrieve the top-most menu
			const Menu *GetCurrentMenu(
				void
			) const XS_WARN_UNUSED_RESULT;

			// retrieve a menu by name
			const Menu *GetMenuByName(
				const char *menuName
			) const XS_WARN_UNUSED_RESULT;

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
