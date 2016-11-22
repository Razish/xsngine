#include <sstream>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSString.h"

namespace String {

	void Concatenate( char *dst, size_t len, const char *src ) {
		size_t destLen = strlen( dst );

		if ( destLen >= len ) {
			throw( XSError( Format( "%s already overflowed", XS_FUNCTION ).c_str() ) );
		}

		Copy( dst + destLen, src, len - destLen);
	}

	void Copy( char *dst, const char *src, size_t len ) {
		if ( !dst ) {
			throw( XSError( Format( "%s NULL dest", XS_FUNCTION ).c_str() ) );
		}

		if ( !src ) {
			throw( XSError( Format( "%s NULL src", XS_FUNCTION ).c_str() ) );
		}

		if ( len < 1 ) {
			throw( XSError( Format( "%s destsize < 1", XS_FUNCTION ).c_str() ) );
		}

		strncpy( dst, src, len - 1 );
		dst[len - 1] = '\0';
	}

	int FormatBuffer( char *dst, size_t len, const char *fmt, ... ) {
		va_list ap;

		va_start( ap, fmt );
		int outLen = vsnprintf( dst, len, fmt, ap );
		va_end( ap );

		if ( outLen >= (signed)len ) {
			throw( XSError( Format( "%s output length %d too short, requires %d bytes.", XS_FUNCTION, len,
				outLen + 1 ).c_str() ) );
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
	}

	// by Evan Teran http://stackoverflow.com/a/236803
	void Split( const std::string &s, char delim, std::vector<std::string> &elems ) {
		std::stringstream ss( s );
		std::string item;

		while ( std::getline( ss, item, delim ) ) {
			elems.push_back( item );
		}
	}

	void Join( const std::vector<std::string> &strings, const std::string &separator, std::string &out ) {
		std::stringstream ss;

		const size_t numStrings = strings.size();
		const size_t last = numStrings - 1;
		for ( size_t i = 0; i < numStrings; ++i ) {
			ss << strings[i];

			if ( i != last ) {
				ss << separator;
			}
		}

		out = ss.str();
	}

	std::string &Strip( std::string &s, const std::string &chars ) {
		s.erase(
			remove_if(
				s.begin(),
				s.end(),
				[&chars]( const char& c ) {
					return chars.find( c ) != std::string::npos;
				}
			),
			s.end()
		);
		return s;
	}

	std::string StripCopy( std::string s, const std::string &chars ) {
		return Strip( s, chars );
	}

	std::string &StripNonDigit( std::string &s ) {
		s.erase(
			remove_if(
				s.begin(),
				s.end(),
				[]( const char &c ) {
					return !isdigit( c );
				}
			),
			s.end()
		);
		return s;
	}

	std::string StripNonDigitCopy( std::string s ) {
		return StripNonDigit( s );
	}

	std::string &StripIfNot( std::string &s, const std::string &allowed ) {
		s.erase(
			remove_if(
				s.begin(),
				s.end(),
				[&allowed]( const char &c ) {
					return allowed.find(c) == std::string::npos;
				}
			),
			s.end()
		);
		return s;
	}

	std::string StripIfNotCopy( std::string s, const std::string &allowed ) {
		return StripIfNot( s, allowed );
	}

} // namespace String
