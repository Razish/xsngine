#pragma once

#include <string>

#include "XSCommon/XSVector.h"
#include "XSRenderer/XSView.h"

namespace XS {

	namespace Client {

		class MenuElement;

		class Menu {
			friend class MenuManager;

		private:
			static uint32_t				 version;

			std::string					 name;
			vector2						 position; // top-left
			vector2						 size; // width/height
			//TODO: don't store these as pointers
			std::vector<MenuElement *>	 elements;

		public:
			const Renderer::View		&view;

			// don't allow default instantiation
			Menu() = delete;
			Menu( const Menu& ) = delete;
			Menu& operator=( const Menu& ) = delete;

			Menu(
				const Renderer::View &view,
				const char *fileName
			);

			void Paint(
				void
			);

			bool MouseButtonEvent(
				const struct MouseButtonEvent &ev
			);

			bool MouseMotionEvent(
				void
			);
		};

	} // namespace Client

} // namespace XS
