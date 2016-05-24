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
			struct Data {
				Renderer::Font	*font;

				Data()
				:	font( nullptr )
				{
				}
			} data;

		public:
			struct Properties {
				bool			centered;
				uint16_t		pointSize;
				std::string		text;
				bool			vertical;

				Properties()
				:	centered( false ),
					pointSize( 16u ),
					text( "" ),
					vertical( false )
				{
				}
			} properties;

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
