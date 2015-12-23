#pragma once

#include "XSClient/XSMenuElement.h"

namespace XS {

	class TokenParser;

	namespace Renderer {
		struct Material;
		class Font;
	}

	namespace Client {

		class MenuElementSlider : public MenuElement {
		private:
			struct {
				Renderer::Material	*thumb = nullptr;
				Renderer::Material	*bar = nullptr;
			} assets;

			vector2			size;
			std::string		cvarName;
			struct {
				real32_t bottom = 0.0, top = 1.0;
			} range;
			std::string		postExecCommand;
			bool			updatingValue = false;

			struct Properties {
				bool			centered = false;
				bool			vertical = false;
				bool			integral = false;
			} properties;

			bool MouseWithinBounds(
				const vector2 &mousePos
			) const XS_WARN_UNUSED_RESULT;

			void ParseProperties(
				TokenParser *parser,
				const char *fileName
			);

			void UpdateValue(
				real32_t frac
			);

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
				const struct MouseButtonEvent &ev
			);

			// handle a mouse motion event
			void MouseMotionEvent(
				void
			);
		};

	} // namespace Client

} // namespace XS
