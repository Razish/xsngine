#pragma once

#include <vector>
#include <string>

namespace XS {

	class File;

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

		~Logger();

		// don't allow default instantiation
		Logger() = delete;
		Logger( const Logger& ) = delete;
		Logger& operator=( const Logger& ) = delete;

		void	Print	( const char *message );
	};

} // namespace XS
