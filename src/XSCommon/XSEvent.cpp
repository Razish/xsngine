#include "XSSystem/XSInclude.h"

#include "SDL2/SDL_keycode.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSError.h"
#include "XSClient/XSKeys.h"

namespace XS {

	namespace Event {

		static Cvar *debug_events = NULL;

		// this is cleared each pump. we may just want to expand
		//	as necessary and set the head to 0 at the end of each pump
		//	to avoid the overhead of clear()
		static std::list<xsEvent_t> events;

		// for debugging
		static const char *eventNames[EVENT_NUM_EVENTS] = {
			"EVENT_KEY",
		};

		void Init( void ) {
			debug_events = Cvar::Create( "debug_events", "0", CVAR_INIT );
		}

		void Queue( eventType_t type, xsEvent_t *ev ) {
			ev->time = 0; // FIXME
			ev->type = type;

			if ( debug_events->GetBool() ) {
				switch( type ) {
				case EVENT_KEY:
					Console::Print( "Event::Queue: %s (%i) key: %i, down: %i\n", eventNames[type], type,
						ev->keyEvent.key, ev->keyEvent.down );
					break;
				default:
					throw( XSError( String::Format( "Event::Queue: Unknown event %i", type ).c_str() ) );
					break;
				}
			}

			events.push_back( *ev );
		}

		void Pump( void ) {
			for ( const auto &it : events ) {
				switch( it.type ) {
					case EVENT_KEY:
						Client::KeyEvent( it.keyEvent.key, it.keyEvent.down );
						break;
					default:
						throw( XSError( String::Format( "Event::Pump: Unknown event %i", it.type ).c_str() ) );
						break;
				}
			}
			events.clear();
		}

	} // Event

} // XS
