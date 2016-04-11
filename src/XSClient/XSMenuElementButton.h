#pragma once

#include "XSClient/XSMenuElement.h"

namespace XS {

	class TokenParser;

	namespace Renderer {
		struct Material;
		class Font;
	}

	namespace Client {

		class MenuElementButton : public MenuElement {

		private:
			struct {
				Renderer::Material	*background = nullptr;
				Renderer::Font		*font = nullptr;
			} assets;

			vector2			size;
			std::string		texture;
			std::string		cmd;
			std::string		text;
			uint16_t		pointSize = 16u;

			struct Properties : public MenuElement::Properties {
				bool			centered = false;

				Properties()
				: MenuElement::Properties() {
				}
			} properties;

			bool MouseWithinBounds(
				const vector2 &mousePos
			) const XS_WARN_UNUSED_RESULT;

			void ParseProperties(
				TokenParser *parser,
				const char *fileName
			);

		public:
			// don't allow default instantiation
			MenuElementButton() = delete;
			MenuElementButton( const MenuElementButton& ) = delete;
			MenuElementButton& operator=( const MenuElementButton& ) = delete;

			MenuElementButton(
				const Menu &parent,
				TokenParser *parser,
				const char *fileName
			);

			// draw the button
			void Paint(
				void
			);

			// handle a keyboard event
			bool KeyboardEvent(
				const struct KeyboardEvent &ev
			) XS_WARN_UNUSED_RESULT;

			// handle a mouse button event
			bool MouseButtonEvent(
				const struct MouseButtonEvent &ev
			) XS_WARN_UNUSED_RESULT;

			// handle a mouse motion event
			bool MouseMotionEvent(
				const struct MouseMotionEvent &ev
			) XS_WARN_UNUSED_RESULT;

			// handle a mouse wheel event
			bool MouseWheelEvent(
				const struct MouseWheelEvent &ev
			) XS_WARN_UNUSED_RESULT;

		};

	} // namespace Client

} // namespace XS
