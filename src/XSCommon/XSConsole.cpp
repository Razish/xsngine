#include "XSCommon/XSConsole.h"
#include "XSCommon/XSFormat.h"

#include <string>
#include <cstdarg>
#include <cstdio>
#include <iostream>

#ifdef _WIN32
	#define VC_EXTRALEAN
	#include <Windows.h>
#endif

namespace XS {

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

		std::cout << str;

		#if defined(_WIN32) && defined( _DEBUG )
			if ( str[0] )
				OutputDebugString( str.c_str() );
		#endif
	}

}; // namespace XS
