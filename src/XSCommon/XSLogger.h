#pragma once

#include "XSCommon/XSFile.h"

namespace XS {

	class Logger {
	private:
		std::vector<std::string>	queue;
		File						*f;
		std::string					filename;
		bool						timestamp;

		void	PrintQueued	( void );
		void	Queue		( std::string &str );

	public:
		Logger( const char *filename, bool timestamp = true )
		: f( nullptr ), filename( filename ), timestamp( timestamp )
		{
		}
		~Logger() {
			delete f;
		}

		// don't allow default instantiation
		Logger() = delete;
		Logger( const Logger& ) = delete;
		Logger& operator=( const Logger& ) = delete;

		void	Print	( const char *message );
	};

} // namespace XS
