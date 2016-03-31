#pragma once

#include "XSClient/XSMenuElement.h"

namespace XS {

	class TokenParser;

	namespace Renderer {
		struct Material;
		class Font;
	}

	namespace Client {

		class MenuElementText : public MenuElement {
		private:
			struct {
				Renderer::Font		*font = nullptr;
			} assets;

			std::string		text;
			uint16_t		pointSize = 16u;

			struct Properties : public MenuElement::Properties {
				bool			centered = false;
				bool			vertical = false;

				Properties()
				: MenuElement::Properties() {
				}
			} properties;

			void ParseProperties(
				TokenParser *parser,
				const char *fileName
			);

		public:
			// don't allow default instantiation
			MenuElementText() = delete;
			MenuElementText( const MenuElementText& ) = delete;
			MenuElementText& operator=( const MenuElementText& ) = delete;

			MenuElementText(
				const Menu &parent,
				TokenParser *parser,
				const char *fileName
			);

			// draw the text
			void Paint(
				void
			);

			// handle a mouse button event
			void MouseButtonEvent(
				const struct MouseButtonEvent &ev
			);

			// handle a mouse motion event
			void MouseMotionEvent(
				void
			);
		};

	} // namespace Client

} // namespace XS
