#pragma once

#include <SDL2/SDL_keycode.h>

//	events are generated throughout the frame and timestamped (i.e. keyboard/mouse input)
//	they are processed at key points in the frame, such as before executing the command buffer and before running a
//		client frame or server frame
//	internal functions are encapsulated in XSEvent.cpp

namespace XS {

	enum EventType {
		UNKNOWN = 0,
		KEY,
		MOUSEBUTTON,
		MOUSEWHEEL,
		NUM_EVENTS
	};

	struct XSEvent {
		EventType	type;
		uint32_t	time;

		union {
			struct {
				SDL_Keycode	key;
				bool		down;
			} keyEvent;

			struct {
				uint8_t	button;
				bool	pressed;
			} mouseButton;

			struct {
				bool		up;
				uint32_t	amount;
			} mouseWheel;
		};

		XSEvent( EventType type )
		: type( type ), time( 0u )
		{
		}

		XSEvent() = delete;
		XSEvent& operator=( const XSEvent& ) = delete;
	};

	namespace Event {

		// initialise the event subsystem
		void Init(
			void
		);

		// add an event to the event queue
		void Queue(
			const XSEvent *ev
		);

		// pump all queued events, executing the necessary functions
		void Pump(
			void
		);

	} // namespace Event

} // namespace XS
