#pragma once

#include <SDL2/SDL_keycode.h>

//	events are generated throughout the frame and timestamped (i.e. keyboard/mouse input)
//	they are processed at key points in the frame, such as before executing the command buffer and before running a
//		client frame or server frame
//	internal functions are encapsulated in XSEvent.cpp

namespace XS {

	enum class EventType {
		Keyboard,
		MouseButton,
		MouseWheel,
		MouseMotion,
	};

	struct KeyboardEvent {
		SDL_Keycode	key;
		bool		down;
	};

	struct MouseButtonEvent {
		uint8_t		button;
		bool		pressed;
	};

	struct MouseWheelEvent {
		bool		up;
		uint32_t	amount;
	};

	struct MouseMotionEvent {
		uint32_t	x;
		uint32_t	y;
	};

	struct XSEvent {
		// don't allow default instantiation
		XSEvent() = delete;
		XSEvent& operator=( const XSEvent& ) = delete;

		EventType	type;
		uint32_t	time;

		union {
			KeyboardEvent		keyboard;
			MouseButtonEvent	mouseButton;
			MouseWheelEvent		mouseWheel;
			MouseMotionEvent	mouseMotion;
		};

		XSEvent( EventType eventType )
		: type( eventType ), time( 0u )
		{
		}
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
