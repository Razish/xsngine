#pragma once

#include <string>

#include "XSCommon/XSVector.h"
#include "XSRenderer/XSView.h"

namespace Client {

	class MenuElement;

	class Menu {

		friend class MenuManager;

	private:
		static uint32_t				 version;

	public:
		std::string					 name;
		vector2						 position; // top-left
		vector2						 size; // width/height
		//TODO: don't store these as pointers
		//TODO: if all elements have a name, store as associative array?
		std::vector<MenuElement *>	 elements;
		MenuElement					*selectedElement;
		const Renderer::View		&view;

		// don't allow default instantiation
		Menu() = delete;
		Menu( const Menu& ) = delete;
		Menu& operator=( const Menu& ) = delete;

		Menu(
			const Renderer::View &view,
			const char *fileName
		);
		~Menu();

		void Paint(
			void
		);

		MenuElement *Find(
			const char *name
		) const XS_WARN_UNUSED_RESULT;

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
