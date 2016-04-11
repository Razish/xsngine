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

			// handle a keyboard event
			virtual bool KeyboardEvent(
				const struct KeyboardEvent &ev
			) XS_WARN_UNUSED_RESULT = 0;

			// handle a mouse button event
			virtual bool MouseButtonEvent(
				const struct MouseButtonEvent &ev
			) XS_WARN_UNUSED_RESULT = 0;

			// handle a mouse motion event
			virtual bool MouseMotionEvent(
				const struct MouseMotionEvent &ev
			) XS_WARN_UNUSED_RESULT = 0;

			// handle a mouse wheel event
			virtual bool MouseWheelEvent(
				const struct MouseWheelEvent &ev
			) XS_WARN_UNUSED_RESULT = 0;

		};

	} // namespace Client

} // namespace XS
