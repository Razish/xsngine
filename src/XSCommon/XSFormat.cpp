#include <string>
#include <vector>
#include <sstream>
#include <cstdarg>
#include <cstdio>

namespace XS {

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
	std::vector<std::string> &split( const std::string &s, char delim, std::vector<std::string> &elems ) {
		std::stringstream ss( s );
		std::string item;

		while ( std::getline( ss, item, delim ) )
			elems.push_back( item );

		return elems;
	}

	std::vector<std::string> split( const std::string &s, char delim ) {
		std::vector<std::string> elems;

		split( s, delim, elems );

		return elems;
	}

}; // namespace XS
