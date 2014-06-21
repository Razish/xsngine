#pragma once

#include "XSCommon/XSFile.h"

namespace XS {

	class Logger {
	private:
		std::vector<std::string> queue;
		void PrintQueued( void );
		void Queue( std::string &str );

		File *f;
		std::string filename;
		bool timestamp;

	public:
		// don't allow default instantiation
		Logger() = delete;
		Logger( const Logger& ) = delete;
		Logger& operator=( const Logger& ) = delete;

		Logger( const char *filename, bool timestamp = true ) : f( nullptr ), filename( filename ), timestamp( timestamp ) {}
		~Logger() { delete f; }

		void Print( const char *message );
	};
}
