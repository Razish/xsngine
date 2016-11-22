#pragma once

#include "XSCommon/XSCommand.h"
#include "XSCommon/XSEvent.h"

struct vector2;

namespace Client {

	namespace Input {

		struct MovementCommand;

		extern vector2 mouseDelta;

		// lock mouse to screen and only recieve relative mouse movement, or behave like a regular window
		void CaptureMouse(
			bool doCapture
		);

		// pass input events to the client system (console -> menus -> binds/ingame)

		// ???
		bool MouseButtonEvent(
			const struct MouseButtonEvent &ev
		) XS_WARN_UNUSED_RESULT;

		// ???
		bool MouseMotionEvent(
			const struct MouseMotionEvent &ev
		) XS_WARN_UNUSED_RESULT;

		// ???
		bool MouseWheelEvent(
			const struct MouseWheelEvent &ev
		) XS_WARN_UNUSED_RESULT;

		// ???
		void CalculateMouseMotion(
			real64_t frameMsec,
			MovementCommand &cmd
		);

	} // namespace Input

} // namespace Client
