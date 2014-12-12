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

	void MessageBuffer::Append( std::string message ) {
		buffer.push_back( message );

		if ( log ) {
			log->Print( message.c_str() );
		}
	}

	bool MessageBuffer::IsEmpty( void ) const {
		return buffer.empty();
	}

	std::vector<std::string> MessageBuffer::GetLines( unsigned int lineCount ) const {
		return std::vector<std::string>( buffer.end() - std::min( buffer.size(), static_cast<size_t>(lineCount) ),
			buffer.end() );
	}

} // namespace XS
