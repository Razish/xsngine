#pragma once

#include <string>

#include "XSCommon/XSVector.h"

namespace XS {

	class TokenParser;
	class Menu;

	namespace Renderer {
		class Font;
	}

	namespace Client {

		class MenuElement {
			friend class Menu;
		private:
			// ...

		protected:
			MenuElement();

			Menu			*parent = nullptr;
			vector2			 position;
			std::string		 name;
			struct Properties {
				bool			hidden = false;
				bool			decorative = false;
			} properties;

			struct Tooltip {
				Renderer::Font	*font = nullptr;
				uint16_t		 pointSize = 16u;
				std::string		 text = "";
				bool			 mouseHovering = false;
				real64_t		 lastMouseTime = 0.0;
				vector2			 lastMousePos = {};
			} tooltip;

		public:
			// determine whether or not events should be consumed by this element
			bool MouseWithinBounds(
				const vector2 &mousePos,
				const vector2 &size
			) const;

			// paint the element on the screen
			virtual void Paint(
				void
			) = 0;

			// parse a tooltip definition
			void ParseTooltip(
				TokenParser *parser,
				const char *fileName
			);

			// draw the tooltip
			void DrawTooltip(
				void
			);

			// handle a mouse button event
			virtual void MouseButtonEvent(
				const struct MouseButtonEvent &ev
			) = 0;

			// handle a mouse motion event
			virtual void MouseMotionEvent(
				void
			) = 0;
		};

	} // namespace Client

} // namespace XS
