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
				Renderer::Material	*thumb;
				Renderer::Material	*bar;
				Renderer::Font		*font;
			} assets;

			vector2			size;
			std::string		cvarName;
			struct {
				real32_t bottom, top;
			} range;
			std::string		postExecCommand;
			bool			updatingValue;

			// tooltip
			uint16_t		pointSize;
			std::string		tooltipText;
			bool			mouseHovering;
			real64_t		lastTooltipTime;
			vector2			lastMousePos;

			struct Properties {
				bool			centered;
				bool			vertical;
				bool			integral;
			} properties;

			bool MouseWithinBounds(
				const vector2 &mousePos
			) const;

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
