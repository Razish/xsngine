#pragma once

namespace XS {

	class Logger {
	private:
		Logger();

		File *f;
		std::string filename;
		bool timestamp;
		bool tryOpen;

	public:
		Logger( const char *filename, bool timestamp = true )
			: f( NULL ), filename( filename ), timestamp( timestamp ), tryOpen( true ) {}
		~Logger() { delete f; }

		void Print( const char *message );
	};
}
