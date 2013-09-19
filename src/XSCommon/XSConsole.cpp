#include "System/XSInclude.h"

#ifdef _WIN32
	#define VC_EXTRALEAN
	#include <Windows.h>
#endif

#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSColours.h"

namespace XS {

	//
	// Public interface
	//

	void Print( std::string fmt, ... ) {
		int size = 100;
		std::string str;
		va_list ap;

		while ( 1 ) {
			str.resize( size );

			va_start( ap, fmt );
			int n = vsnprintf( (char *)str.c_str(), size, fmt.c_str(), ap );
			va_end( ap );

			if ( n > -1 && n < size ) {
				str.resize( n );
				break;
			}
			if ( n > -1 )
				size = n + 1;
			else
				size *= 2;
		}

		//TODO: strip colours?
		std::cout << str;
		console.Append( str.c_str() );

		#if defined(_WIN32) && defined( _DEBUG )
			if ( str[0] )
				OutputDebugString( str.c_str() );
		#endif
	}

	//
	// Console class
	//	Access via XS::console.blah()
	//
	Console console;

	// private member functions
	short Console::EncodeCharacter( char lastColour, char c ) {
		return (lastColour << 8) | c;
	}

	// public member functions
	void Console::Display( void ) {
		// ...
	}

	void Console::Append( const char *text ) {
		size_t len = strlen( text );
		consoleText_s tmp[CONSOLE_BUFFER_SIZE];
		char lastColour = COLOUR_WHITE;

		for ( size_t i=0; i<len; i++ ) {
			tmp[i].raw = EncodeCharacter( lastColour, text[i] );
		}
	}

}; // namespace XS
