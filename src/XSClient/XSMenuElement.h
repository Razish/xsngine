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

		// a MenuElement is intended to be extended by more specialised element types, but does not enforce this
		// each MenuElement has an associated parent Menu
		class MenuElement {

		private:
			// ...

		public:
			const Menu		&parent;

			// common properties
			struct Common {
				bool			decorative;
				bool			hidden;
				std::string		name;
				vector2			position;

				Common()
				:	decorative( false ),
					hidden( false ),
					name( "" ),
					position( vector2{ 0.0f, 0.0f } )
				{
				}
			} common;

			struct Tooltip {
				Renderer::Font	*font;
				vector2			 lastMousePos;
				real64_t		 lastMouseTime;
				bool			 mouseHovering;
				uint16_t		 pointSize;
				std::string		 text;

				Tooltip()
				:	font( nullptr ),
					lastMousePos( vector2{ 0.0f, 0.0f } ),
					lastMouseTime( 0.0 ),
					mouseHovering( false ),
					pointSize( 16u ),
					text( "" )
				{
				}
			} tooltip;

			MenuElement(
				const Menu &parent
			);

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
