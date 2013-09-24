#pragma once

// XS event pump

//	events are generated throughout the frame and timestamped (i.e. keyboard/mouse input)
//	They are processed at key points in the frame, such as before executing the command buffer and before running a client frame or server frame
//	Internal functions are encapsulated in XSEvent.cpp

namespace XS {

	namespace Event {

		enum Type {
			KEYEVENT=0,
			NUM_EVENTS
		};

		struct XSEvent {
			// these will be set internally
			Type	type;
			uint32_t	time;

			union {
				struct {
					SDL_Keycode	key;
					bool		down;
				} keyEvent;
			};
		};

		void Init( void );
		void Queue( Type type, XSEvent ev );
		void Pump( void );

	} // Event

} // XS
