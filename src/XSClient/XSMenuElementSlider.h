#pragma once

#include "XSClient/XSMenuElement.h"

class TokenParser;

namespace Renderer {
	struct Material;
	class Font;
}

namespace Client {

	class MenuElementSlider : public MenuElement {

	private:
		struct Data {
			Renderer::Material	*bar;
			Renderer::Material	*thumb;
			bool				 updatingValue;

			Data()
			:	bar( nullptr ),
				thumb( nullptr ),
				updatingValue( false )
			{
			}
		} data;

		// ???
		void UpdateValue(
			real32_t frac
		);

	public:
		struct Properties {
			bool			centered;
			std::string		cvarName;
			bool			integral;
			std::string		postExecCommand;
			vector2			size;
			bool			vertical;

			struct Range {
				real32_t bottom;
				real32_t top;

				Range()
				:	bottom( 0.0 ),
					top( 1.0 )
				{
				}
			} range;

			Properties()
			:	centered( false ),
				cvarName( "" ),
				integral( false ),
				postExecCommand( "" ),
				size( { 0.0f, 0.0f } ),
				vertical( false )
			{
			}
		} properties;

		// don't allow default instantiation
		MenuElementSlider() = delete;
		MenuElementSlider( const MenuElementSlider& ) = delete;
		MenuElementSlider& operator=( const MenuElementSlider& ) = delete;

		MenuElementSlider(
			const Menu &parent,
			TokenParser *parser,
			const char *fileName
		);

		// draw the slider
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
