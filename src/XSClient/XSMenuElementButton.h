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
				Renderer::Material	*background;
				Renderer::Font		*font;
			} assets;

			std::string		texture;
			std::string		cmd;
			std::string		text;
			uint16_t		pointSize;

			struct Properties {
				bool			centered;
				bool			vertical;
			} properties;

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
				TokenParser *parser,
				const char *fileName
			);

			// draw the slider
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
