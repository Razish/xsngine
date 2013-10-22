#pragma once

//
// XS event pump
//
//	events are generated throughout the frame and timestamped (i.e. keyboard/mouse input)
//	They are processed at key points in the frame, such as before executing the command buffer and before running a
//		client frame or server frame
//	Internal functions are encapsulated in XSEvent.cpp
//

namespace XS {

	enum eventType_t {
		EVENT_KEY=0,
		EVENT_NUM_EVENTS
	};

	struct xsEvent_t {
		// these will be set internally
		eventType_t	type;
		uint32_t	time;

		union {
			struct {
				SDL_Keycode	key;
				bool		down;
			} keyEvent;
		};
	};

	namespace Event {

		void Init( void );
		void Queue( eventType_t type, xsEvent_t *ev );
		void Pump( void );

	} // namespace Event

} // namespace XS
