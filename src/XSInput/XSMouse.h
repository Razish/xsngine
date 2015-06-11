#pragma once

#include "XSCommon/XSCommand.h"
#include "XSCommon/XSEvent.h"

namespace XS {

	struct vector2;

	namespace Client {

		namespace Input {

			// lock mouse to screen and only recieve relative mouse movement, or behave like a regular window
			void CaptureMouse(
				bool capture
			);

			// pass input events to the client system (console -> menus -> binds/ingame)
			void MouseWheelEvent(
				const struct MouseWheelEvent &ev
			);

			// ???
			void MouseButtonEvent(
				const struct MouseButtonEvent &ev
			);

			// ???
			void MouseMotionEvent(
				const struct MouseMotionEvent &ev
			);

		} // namespace Input

	} // namespace Client

} // namespace XS
