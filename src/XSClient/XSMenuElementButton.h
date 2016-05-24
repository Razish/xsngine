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
			struct Data {
				Renderer::Material	*background;
				Renderer::Font		*font;

				Data()
				:	background( nullptr ),
					font( nullptr )
				{
				}
			} data;

		public:
			struct Properties {
				bool			centered;
				std::string		cmd;
				uint16_t		pointSize;
				vector2			size;
				std::string		text;
				std::string		texture;

				Properties()
				:	centered( false ),
					cmd( "" ),
					pointSize( 16u ),
					size( vector2{ 0.0f, 0.0f } ),
					text( "" ),
					texture( "" )
				{
				}
			} properties;

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
