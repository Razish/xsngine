#include "XSCommon/XSEvent.h"
#include "XSCommon/XSConsole.h"

#include <list>

namespace XS {

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

			Print( "Pushing event %s (%i) values %i and %i\n", eventNames[type], type, ev.data.value1, ev.data.value2 );
			events.push_back( ev );
		}

		void EventPump( void ) {
			for ( auto it=events.begin(); it != events.end(); ++it ) {
				Print( "Pumping event %s (%i) values %i and %i\n", eventNames[(*it).type], it->type, it->data.value1, it->data.value2 );
			}
			events.clear();
		}

	} // Common

} // XS
