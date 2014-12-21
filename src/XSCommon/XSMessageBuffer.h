#pragma once

#include <vector>
#include <string>

namespace XS {

	class Logger;

	class MessageBuffer {
	private:
		std::vector<std::string> buffer;
		Logger *log;

	public:
		MessageBuffer( const char *logfile );
		~MessageBuffer();

		// don't allow default instantiation
		MessageBuffer() = delete;
		MessageBuffer( const MessageBuffer& ) = delete;
		MessageBuffer& operator=( const MessageBuffer& ) = delete;

		void						Append		( std::string message );
		bool						IsEmpty		( void ) const;
		std::vector<std::string>	GetLines	( unsigned int lineCount = 10 ) const;
	};

} // namespace XS
