#pragma once

#include "XSCommon/XSCommand.h"
#include "XSCommon/XSEvent.h"

namespace XS {

	struct vector2;
	struct MovementCommand;

	namespace Client {

		extern vector2 mouseDelta;

		// pass input events to the client system (console -> menus -> binds/ingame)
		void MouseWheelEvent(
			const MouseWheelEvent &ev
		);
		void MouseButtonEvent(
			const MouseButtonEvent &ev
		);
		void MouseMotionEvent(
			const MouseMotionEvent &ev
		);

		void CalculateMouseMotion(
			real64_t frameMsec,
			MovementCommand &cmd
		);

	} // namespace Client

} // namespace XS
