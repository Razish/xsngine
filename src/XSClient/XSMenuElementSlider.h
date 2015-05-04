#pragma once

#include "XSClient/XSMenuElement.h"

namespace XS {

	class TokenParser;

	namespace Renderer {
		struct Material;
	}

	namespace Client {

		class MenuElementSlider : public MenuElement {
		private:
			static struct Assets {
				Renderer::Material	*thumb;
				Renderer::Material	*bar;
			} assets;

			std::string		cvarName;
			struct {
				real32_t		bottom;
				real32_t		top;
			} range;
			std::string		postExecCommand;

			struct Properties {
				bool			centered;
				bool			vertical;
			} properties;

			void ParseProperties(
				TokenParser *parser,
				const char *fileName
			);

			void UpdateValue(
				real32_t frac
			);

			bool updatingValue;

		public:
			// don't allow default instantiation
			MenuElementSlider() = delete;
			MenuElementSlider( const MenuElementSlider& ) = delete;
			MenuElementSlider& operator=( const MenuElementSlider& ) = delete;

			MenuElementSlider(
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
