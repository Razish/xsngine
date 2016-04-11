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

			bool KeyboardEvent(
				const struct KeyboardEvent &ev
			) XS_WARN_UNUSED_RESULT;

			bool MouseButtonEvent(
				const struct MouseButtonEvent &ev
			) XS_WARN_UNUSED_RESULT;

			bool MouseMotionEvent(
				const struct MouseMotionEvent &ev
			) XS_WARN_UNUSED_RESULT;

			bool MouseWheelEvent(
				const struct MouseWheelEvent &ev
			) XS_WARN_UNUSED_RESULT;

		};

	} // namespace Client

} // namespace XS
