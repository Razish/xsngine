#include <iostream>
#include <sstream>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSMessageBuffer.h"
#include "XSCommon/XSCvar.h"

#if defined(XS_OS_WINDOWS) && defined(_DEBUG)
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#ifndef NOMINMAX
		#define NOMINMAX /* Don't define min() and max() */
	#endif
	#include <Windows.h>
#endif

Console console = {};

Console::Console()
:	indentation( 0u )
{
	buffer = new MessageBuffer( "console.log" );
}

void Console::Print( PrintLevel printLevel, const char *fmt, ... ) {
	if ( printLevel == PrintLevel::Debug ) {
		if ( !XS_DEBUG_BUILD ) {
			return;
		}
	}
	else if ( printLevel == PrintLevel::Developer ) {
		if ( Common::com_developer->GetBool() == false ) {
			return;
		}
	}

	size_t size = 128;
	std::string str;
	va_list ap;

	while ( 1 ) {
		str.resize( size );

		va_start( ap, fmt );
		int n = vsnprintf( (char *)str.c_str(), size, fmt, ap );
		va_end( ap );

		if ( n > -1 && n < (signed)size ) {
			str.resize( n );
			break;
		}
		if ( n > -1 ) {
			size = n + 1;
		}
		else {
			size *= 1.5;
		}
	}

	// preprocess the string (e.g. split by \n)
	std::istringstream ss( str );
	std::string line;
	while ( std::getline( ss, line ) ) {
		//FIXME: care about printing twice on same line
		std::string finalOut = "";
		for ( int32_t i = 0; i < indentation; i++ ) {
			finalOut += "  ";
		}
		finalOut += line;

		//TODO: strip colours?
		fprintf( stdout, "%s\n", finalOut.c_str() );
		buffer->Append( finalOut );

	#if defined(XS_OS_WINDOWS) && defined(_DEBUG)
		if ( !finalOut.empty() ) {
			OutputDebugString( finalOut.c_str() );
		}
	#endif
	}
}
