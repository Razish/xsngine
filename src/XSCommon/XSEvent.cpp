#include <list>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSError.h"
#include "XSInput/XSKeys.h"

namespace XS {

	namespace Event {

		static Cvar *debug_events = nullptr;

		// this is cleared each pump. we may just want to expand
		//	as necessary and set the head to 0 at the end of each pump
		//	to avoid the overhead of clear()
		static std::list<XSEvent> events;

		// for debugging
		static const char *eventNames[EventType::NUM_EVENTS] = {
			"EventType::Unknown",
			"EventType::KEY",
			"EventType::MOUSEBUTTON",
			"EventType::MOUSEWHEEL",
		};

		void Init( void ) {
			debug_events = Cvar::Create( "debug_events", "0", "Print debug messages for input events", CVAR_INIT );
		}

		void Queue( const XSEvent *ev ) {
			if ( debug_events->GetBool() ) {
				switch( ev->type ) {
				case EventType::KEY: {
					console.Print( PrintLevel::Debug, "%s %s (%i) key: %i, down: %i\n",
						XS_FUNCTION,
						eventNames[ev->type],
						ev->type,
						ev->keyEvent.key,
						ev->keyEvent.down
					);
				} break;

				case EventType::MOUSEBUTTON: {
					// ...
				} break;

				case EventType::MOUSEWHEEL: {
					// ...
				} break;

				default: {
					//TODO: test if stack unwinding/object destruction breaks this message
					throw( XSError( String::Format( "%s Unknown event %i", XS_FUNCTION, ev->type ).c_str() ) );
				} break;
				}
			}

			events.push_back( *ev );
		}

		void Pump( void ) {
			for ( const auto &it : events ) {
				switch( it.type ) {
				case EventType::KEY: {
					Client::KeyEvent( it.keyEvent.key, it.keyEvent.down );
				} break;

				case EventType::MOUSEBUTTON: {
					// ...
				} break;

				case EventType::MOUSEWHEEL: {
					// ...
				} break;

				default: {
					throw( XSError( String::Format( "%s Unknown event %i", XS_FUNCTION, it.type ).c_str() ) );
				} break;
				}
			}
			events.clear();
		}

	} // namespace Event

} // namespace XS
