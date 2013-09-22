#include "System/XSInclude.h"
#include "XSCommon/XSString.h"

namespace XS {

	namespace String {

		void Concatenate( char *dst, size_t len, const char *src ) {
			size_t l1 = strlen( dst );

			if ( l1 >= len )
				throw( "XS::String::Concatenate: already overflowed" );

			Copy( dst+l1, src, len-l1 );
		}

		// safe strncpy that ensures a trailing zero
		void Copy( char *dst, const char *src, size_t len ) {
			if ( !dst )
				throw( "XS::String::Copy: NULL dest" );

			if ( !src )
				throw( "XS::String::Copy: NULL src" );

			if ( len < 1 )
				throw( "XS::String::Copy: destsize < 1" );

			strncpy( dst, src, len-1 );
			dst[len-1] = 0;
		}

		// by Erik Aronesty http://stackoverflow.com/a/8098080
		std::string Format( const std::string fmt, ... ) {
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
					return str;
				}
				if ( n > -1 )
					size = n + 1;
				else
					size *= 2;
			}
			return str;
		}

		// by Evan Teran http://stackoverflow.com/a/236803
		std::vector<std::string> &Split( const std::string &s, char delim, std::vector<std::string> &elems ) {
			std::stringstream ss( s );
			std::string item;

			while ( std::getline( ss, item, delim ) )
				elems.push_back( item );

			return elems;
		}

		std::vector<std::string> Split( const std::string &s, char delim ) {
			std::vector<std::string> elems;

			Split( s, delim, elems );

			return elems;
		}

	}; // namespace String

}; // namespace XS
