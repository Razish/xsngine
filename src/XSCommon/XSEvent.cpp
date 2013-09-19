#include "System/XSInclude.h"

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSEvent.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSCvar.h"
#include "XSClient/XSKeys.h"

namespace XS {

	extern Cvar *debug_events;

	namespace Common {

		// this is cleared each pump. we may just want to expand
		//	as necessary and set the head to 0 at the end of each pump
		//	to avoid the overhead of clear()
		static std::list<XSEvent> events;

		// for debugging
		static const char *eventNames[NUM_EVENTS] = {
			"KEYEVENT",
		};

		void QueueEvent( EventType type, XSEvent ev ) {
			ev.time = 0; // FIXME
			ev.type = type;

			if ( debug_events->Bool() ) {
				switch( type ) {
				case KEYEVENT:
					Print( "QueueEvent: %s (%i) key: %i, down: %i\n", eventNames[type], type, ev.keyEvent.key, ev.keyEvent.down );
					break;
				default:
					throw( String::Format( "QueueEvent: Unknown event %i", type ).c_str() );
					break;
				}
			}

			events.push_back( ev );
		}

		void EventPump( void ) {
			for ( auto it=events.begin(); it != events.end(); ++it ) {
				switch( it->type ) {
					case KEYEVENT:
						Client::KeyEvent( it->keyEvent.key, it->keyEvent.down );
						break;
					default:
						throw( String::Format( "EventPump: Unknown event %i", it->type ).c_str() );
						break;
				}
			}
			events.clear();
		}

	} // Common

} // XS
