#pragma once

#include <string>
#include <vector>

#include "XSCommon/XSLogger.h"
#include "XSCommon/XSFile.h"

namespace XS {

	class MessageBuffer {
	private:
		std::vector<std::string> buffer;
		Logger *log;

	public:
		MessageBuffer( const char *logfile = NULL );
		~MessageBuffer() { delete log; }
		void Append( std::string message );
		bool IsEmpty( void ) const;
		std::vector<std::string> GetLines( unsigned int lineCount = 10 );
	};

} // namespace XS
