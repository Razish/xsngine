#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSMessageBuffer.h"
#include "XSCommon/XSLogger.h"

namespace XS {

	MessageBuffer::MessageBuffer( const char *logfile ) {
		if ( logfile ) {
			log = new Logger( logfile );
		}
		else {
			log = nullptr;
		}
	}
	MessageBuffer::~MessageBuffer() {
		delete log;
	}

	void MessageBuffer::Append( const Message &message ) {
		buffer.push_back( message );

		if ( log ) {
			log->Print( message.c_str() );
		}
	}

	bool MessageBuffer::IsEmpty( void ) const {
		return buffer.empty();
	}

	void MessageBuffer::Clear( void ) {
		buffer.clear();
	}

} // namespace XS
