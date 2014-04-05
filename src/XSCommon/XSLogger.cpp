#include "XSSystem/XSInclude.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSLogger.h"
#include "XSCommon/XSString.h"

namespace XS {

	void Logger::Print( const char *message ) {
		if ( tryOpen && !f ) {
			f = new File( filename.c_str(), FileMode::APPEND );
			if ( !f->open ) {
				delete f;
				f = NULL;
				tryOpen = false;
			}
		}

		if ( !f )
			return;

		// create timestamp
		if ( timestamp ) {
			char buf[128];
			time_t rawtime;
			time( &rawtime );
			strftime( buf, sizeof(buf), "[%Y-%m-%d] [%H:%M:%S] ", localtime( &rawtime ) );
			f->AppendString( buf );
		}

		f->AppendString( message );
	}
} // namespace XS
