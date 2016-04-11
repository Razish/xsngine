#include "XSCommon/XSCommon.h"
#include "XSCommon/XSLogger.h"
#include "XSCommon/XSFile.h"

namespace XS {

	Logger::~Logger() {
		delete f;
	}

	void Logger::PrintQueued( void ) {
		for ( const std::string &str : queue ) {
			f->AppendString( str.c_str() );
		}
		queue.clear();
	}

	void Logger::Queue( std::string &str ) {
		queue.push_back( str );
	}

	void Logger::Print( const char *message ) {
		std::string out;

		// create timestamp
		if ( addTimestamp ) {
			char buf[128];
			time_t rawtime;
			time( &rawtime );
			strftime( buf, sizeof(buf), "[%Y-%m-%d] [%H:%M:%S] ", localtime( &rawtime ) );
			out += buf;
		}

		out += message;

		if ( f ) {
			f->AppendString( out.c_str() );
		}
		else {
			// haven't opened the file yet, see if filesystem is available
			f = new File( filename.c_str(), FileMode::Append );
			if ( f->isOpen ) {
				// success, print what we've missed so far
				Queue( out );
				PrintQueued();
			}
			else {
				// can't open yet, queue it
				delete f;
				f = nullptr;
				Queue( out );
				return;
			}
		}
	}

} // namespace XS
