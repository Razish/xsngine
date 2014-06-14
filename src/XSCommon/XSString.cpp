#include "XSSystem/XSInclude.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSError.h"

namespace XS {

	namespace String {

		void Concatenate( char *dst, size_t len, const char *src ) {
			size_t l1 = strlen( dst );

			if ( l1 >= len ) {
				throw( XSError( "XS::String::Concatenate: already overflowed" ) );
			}

			Copy( dst+l1, src, len-l1 );
		}

		// safe strncpy that ensures a trailing zero
		void Copy( char *dst, const char *src, size_t len ) {
			if ( !dst ) {
				throw( XSError( "XS::String::Copy: NULL dest" ) );
			}

			if ( !src ) {
				throw( XSError( "XS::String::Copy: NULL src" ) );
			}

			if ( len < 1 ) {
				throw( XSError( "XS::String::Copy: destsize < 1" ) );
			}

			strncpy( dst, src, len-1 );
			dst[len-1] = 0;
		}

		// safe sprintf-like
		int FormatBuffer( char *dst, size_t len, const char *fmt, ... ) {
			va_list ap;

			va_start( ap, fmt );
			int outLen = vsnprintf( dst, len, fmt, ap );
			va_end( ap );

			if ( outLen >= (signed)len ) {
				throw( XSError( String::Format( "FormatBuffer: Output length %d too short, requires %d bytes.", len,
					outLen+1 ).c_str() ) );
			}

			return outLen;
		}

		// strncmp
		int CompareCase( const char *s1, const char *s2, size_t len ) {
			// bail early on NULL strings
			if ( !s1 ) {
				if ( !s2 ) {
					return 0;
				}
				else {
					return -1;
				}
			}
			else if ( !s2 ) {
				return 1;
			}

			int c1, c2;
			do {
				// strings considered equal if len reaches 0
				if ( !len-- ) {
					return 0;
				}

				c1 = *s1++;
				c2 = *s2++;

				if ( c1 != c2 ) {
					return (c1 < c2) ? -1 : 1;
				}
			} while ( c1 );

			return 0;
		}

		// stricmpn
		int Compare( const char *s1, const char *s2, size_t len ) {
			// bail early on NULL strings
			if ( !s1 ) {
				if ( !s2 ) {
					return 0;
				}
				else {
					return -1;
				}
			}
			else if ( !s2 ) {
				return 1;
			}

			int c1, c2;
			do {
				// strings considered equal if len reaches 0
				if ( !len-- ) {
					return 0;
				}

				c1 = *s1++;
				c2 = *s2++;

				if ( c1 != c2 ) {
					// convert to uppercase
					if ( c1 >= 'a' && c1 <= 'z' ) {
						c1 -= ('a' - 'A');
					}
					if ( c2 >= 'a' && c2 <= 'z' ) {
						c2 -= ('a' - 'A');
					}

					if ( c1 != c2 ) {
						return (c1 < c2) ? -1 : 1;
					}
				}
			} while ( c1 );

			return 0;
		}

		// by Erik Aronesty http://stackoverflow.com/a/8098080
		std::string Format( const char *fmt, ... ) {
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
					return str;
				}
				if ( n > -1 ) {
					size = n + 1;
				}
				else {
					size *= 2;
				}
			}
			return str;
		}

		// by Evan Teran http://stackoverflow.com/a/236803
		std::vector<std::string> &Split( const std::string &s, char delim, std::vector<std::string> &elems ) {
			std::stringstream ss( s );
			std::string item;

			while ( std::getline( ss, item, delim ) ) {
				elems.push_back( item );
			}

			return elems;
		}

		std::vector<std::string> Split( const std::string &s, char delim ) {
			std::vector<std::string> elems;

			Split( s, delim, elems );

			return elems;
		}

		std::string Join( const std::vector<std::string> &strings, const std::string &separator ) {
			std::stringstream ss;

			const auto &end = strings.end();
			for ( const auto &it : strings ) {
				ss << it;
				if ( it != *end ) {
					ss << separator;
				}
			}

			return ss.str();
		}

	} // namespace String

} // namespace XS
