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

			struct Properties {
				bool			centered = false;
				bool			vertical = false;
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
				TokenParser *parser,
				const char *fileName
			);

			// draw the text
			void Paint(
				void
			);

			// handle a mouse button event
			void MouseButtonEvent(
				const struct MouseButtonEvent &ev,
				const vector2 &cursorPos
			);

			// handle a mouse motion event
			void MouseMotionEvent(
				const vector2 &cursorPos
			);
		};

	} // namespace Client

} // namespace XS
