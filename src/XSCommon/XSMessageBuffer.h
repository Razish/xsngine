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
		~MessageBuffer() {
			delete log;
		}

		// don't allow default instantiation
		MessageBuffer() = delete;
		MessageBuffer( const MessageBuffer& ) = delete;
		MessageBuffer& operator=( const MessageBuffer& ) = delete;

		void						Append		( std::string message );
		bool						IsEmpty		( void ) const;
		std::vector<std::string>	GetLines	( unsigned int lineCount = 10 ) const;
	};

} // namespace XS
