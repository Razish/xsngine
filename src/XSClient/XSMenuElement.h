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
			vector2			 size;
			std::string		 name;
			struct Properties {
				bool			hidden;
			} properties;

		public:
			virtual ~MenuElement() = 0;

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
