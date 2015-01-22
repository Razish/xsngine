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

	std::vector<std::string> MessageBuffer::FetchLines( uint32_t start, uint32_t count ) const {
		const uint32_t size = GetNumLines();
		const uint32_t begin = std::min( size, start );
		const uint32_t end = std::min( begin + count, size );

		return std::vector<std::string>( buffer.begin() + begin, buffer.begin() + end );
	}

	uint32_t MessageBuffer::GetNumLines( void ) const {
		return static_cast<uint32_t>( buffer.size() );
	}

} // namespace XS
