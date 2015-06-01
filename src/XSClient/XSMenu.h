#pragma once

#include <string>

#include "XSCommon/XSVector.h"

namespace XS {

	namespace Client {

		class MenuElement;

		class Menu {
			friend class MenuManager;
		private:
			static uint32_t				version;

			std::string					name;
			vector2						position; // top-left
			vector2						size; // width/height
			std::vector<MenuElement *>	elements;

		public:
			// don't allow default instantiation
			Menu() = delete;
			Menu( const Menu& ) = delete;
			Menu& operator=( const Menu& ) = delete;

			Menu(
				const char *fileName
			);

			void Paint(
				void
			);

			bool MouseButtonEvent(
				const struct MouseButtonEvent &ev,
				const vector2 &cursorPos
			);

			bool MouseMotionEvent(
				const vector2 &cursorPos
			);
		};

	} // namespace Client

} // namespace XS
