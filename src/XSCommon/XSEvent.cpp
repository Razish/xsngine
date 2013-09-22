#include "System/XSInclude.h"

#include "SDL2/SDL_keycode.h"

#include "XSCommon/XSEvent.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSCvar.h"
#include "XSClient/XSKeys.h"

namespace XS {

	namespace Event {

		static Cvar *debug_events = NULL;

		// this is cleared each pump. we may just want to expand
		//	as necessary and set the head to 0 at the end of each pump
		//	to avoid the overhead of clear()
		static std::list<XSEvent> events;

		// for debugging
		static const char *eventNames[NUM_EVENTS] = {
			"KEYEVENT",
		};

		void Init( void ) {
			debug_events = Cvar::Create( "debug_events", "0", Cvar::INIT );
		}

		void Queue( Type type, XSEvent ev ) {
			ev.time = 0; // FIXME
			ev.type = type;

			if ( debug_events->GetBool() ) {
				switch( type ) {
				case KEYEVENT:
					Print( "Event::Queue: %s (%i) key: %i, down: %i\n", eventNames[type], type, ev.keyEvent.key, ev.keyEvent.down );
					break;
				default:
					throw( String::Format( "Event::Queue: Unknown event %i", type ) );
					break;
				}
			}

			events.push_back( ev );
		}

		void Pump( void ) {
			for ( auto it=events.begin(); it != events.end(); ++it ) {
				switch( it->type ) {
					case KEYEVENT:
						Client::KeyEvent( it->keyEvent.key, it->keyEvent.down );
						break;
					default:
						throw( String::Format( "Event::Pump: Unknown event %i", it->type ) );
						break;
				}
			}
			events.clear();
		}

	} // Common

} // XS
