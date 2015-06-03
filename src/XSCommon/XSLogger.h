#pragma once

#include <vector>
#include <string>

namespace XS {

	class File;

	class Logger {
	private:
		std::vector<std::string>	queue;
		File						*f = nullptr;
		std::string					filename = "";
		bool						timestamp = false;

		// write queued messages out to file once we have one open
		void PrintQueued(
			void
		);

		// add a message to the print queue
		// when the filesystem becomes available, it will be written to disk
		void Queue(
			std::string &str
		);

	public:
		// don't allow default instantiation
		Logger() = delete;
		Logger( const Logger& ) = delete;
		Logger& operator=( const Logger& ) = delete;

		Logger( const char *logFilename, bool addTimestamp = true )
		: f( nullptr ), filename( logFilename ), timestamp( addTimestamp )
		{
		}

		~Logger();

		// write a message to the log file, or queue it if filesystem is not available
		void Print(
			const char *message
		);
	};

} // namespace XS
