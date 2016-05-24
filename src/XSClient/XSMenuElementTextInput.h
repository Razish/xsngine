#pragma once

#include "XSClient/XSMenuElement.h"

namespace XS {

	class TokenParser;

	namespace Renderer {
		struct Material;
		class Font;
	}

	namespace Client {

		class MenuElementTextInput : public MenuElement {
			friend class MenuElementButton;

		private:
			void Clear(
				void
			);

		protected:
			struct Data {
				Renderer::Material	*background;
				uint32_t			 cursorPos;
				Renderer::Font		*font;
				std::string			 inputText;
				uint32_t			 numChars;

				Data()
				:	background( nullptr ),
					cursorPos( 0u ),
					font( nullptr ),
					inputText( "" ),
					numChars( 0u )
				{
				}
			} data;

		public:
			struct Properties {
				bool			centered;
				vector2			size;
				std::string		texture;
				uint16_t		pointSize;

				Properties()
				:	centered( false ),
					size( vector2{ 0.0f, 0.0f } ),
					texture( "" ),
					pointSize( 16u )
				{
				}
			} properties;

			// don't allow default instantiation
			MenuElementTextInput() = delete;
			MenuElementTextInput( const MenuElementTextInput& ) = delete;
			MenuElementTextInput& operator=( const MenuElementTextInput& ) = delete;

			MenuElementTextInput(
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
