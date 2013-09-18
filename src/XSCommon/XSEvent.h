#pragma once

#include "System/XSTypes.h"

// XS event pump

//	events are generated throughout the frame and timestamped (i.e. keyboard/mouse input)
//	They are processed at key points in the frame, such as before executing the command buffer and before running a client frame or server frame

namespace XS {

	namespace Common {

		enum EventType {
			KEYEVENT=0,
		};

		void QueueEvent( EventType ev, uint32_t value1, uint32_t value2 );

	} // Common

} // XS
