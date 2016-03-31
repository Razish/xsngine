#pragma once

#include <string>

#include "XSCommon/XSVector.h"
#include "XSClient/XSMenu.h"
#include "XSRenderer/XSView.h"

namespace XS {

	class TokenParser;

	namespace Renderer {
		class Font;
	}

	namespace Client {

		// a MenuElement is an abstract base class for more derived/specialised types and as such should have no (or
		//	minimal) data attached to it
		// currently there is some metadata as-well as tooltip data
		class MenuElement {
			friend class Menu;

		private:
			// ...

		protected:
			MenuElement(
				const Menu &parent
			);

			const Menu		&parent;
			vector2			 position;
			std::string		 name;

			// abstract base class for properties
			struct Properties {
				bool			hidden = false;
				bool			decorative = false;
			};

			struct Tooltip {
				Renderer::Font	*font = nullptr;
				uint16_t		 pointSize = 16u;
				std::string		 text = "";
				bool			 mouseHovering = false;
				real64_t		 lastMouseTime = 0.0;
				vector2			 lastMousePos;
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
