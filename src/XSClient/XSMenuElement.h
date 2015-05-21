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

			// get the size of the element
			//	this is used to check if menu button events should be passed to the element
			virtual const vector2 *GetSize(
				void
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
