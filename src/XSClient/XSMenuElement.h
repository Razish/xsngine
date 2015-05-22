#pragma once

#include <string>

namespace XS {

	class TokenParser;
	class Menu;

	namespace Client {

		class MenuElement {
			friend class Menu;
		private:
			// ...

		protected:
			MenuElement();

			Menu			*parent;
			vector2			 position;
			std::string		 name;
			struct Properties {
				bool			hidden;
				bool			decorative;
			} properties;

		public:
			virtual ~MenuElement() = 0;

			// determine whether or not events should be consumed by this element
			bool MouseWithinBounds(
				const vector2 &mousePos,
				const vector2 &size
			) const;

			// paint the element on the screen
			virtual void Paint(
				void
			) = 0;

			// handle a mouse button event
			virtual void MouseButtonEvent(
				const struct MouseButtonEvent &ev,
				const vector2 &cursorPos
			) = 0;

			// handle a mouse motion event
			virtual void MouseMotionEvent(
				const vector2 &cursorPos
			) = 0;
		};

	} // namespace Client

} // namespace XS
