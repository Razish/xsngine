#include "XSCommon/XSCommon.h"
#include "XSCommon/XSTokenParser.h"

namespace XS {

	TokenParser::TokenParser( const char *buffer )
	: numLines( 1 ), data( buffer )
	{
		token[0] = '\0';
	}

	uint32_t TokenParser::GetCurrentLine( void ) const {
		return numLines;
	}

	bool TokenParser::SkipWhitespace( bool *hasNewLines ) {
		char c;
		while ( (c = *data) <= ' ' ) {
			if ( !c ) {
				return false;
			}

			if ( c == '\n' ) {
				numLines++;
				if ( hasNewLines ) {
					*hasNewLines = true;
				}
			}
			data++;
		}
		return true;
	}

	bool TokenParser::SkipLine( void ) {
		char c;
		while ( (c = *data) != '\0' ) {
			if ( c == '\n' ) {
				data++;
				numLines++;
				return true;
			}
			data++;
		}
		return false;
	}

	const char *TokenParser::ParseToken( bool allowLineBreaks ) {
		bool hasNewLines = false;

		token[0] = '\0';

		if ( !data ) {
			return token;
		}

		char c = '\0';
		while ( 1 ) {
			// skip whitespace
			SkipWhitespace( &hasNewLines );

			if ( !data ) {
				return token;
			}
			if ( hasNewLines && !allowLineBreaks ) {
				return token;
			}

			c = *data;

			if ( c == '/' && data[1] == '/' ) {
				// skip double slash comments
				//data += 2;
				while ( *data && *data != '\n' ) {
					data++;
				}
			}

			else if ( c == '/' && data[1] == '*' ) {
				// skip block comments
				//data += 2;
				while ( *data && (*data != '*' || data[1] != '/') ) {
					data++;
				}
				if ( *data ) {
					data += 2;
				}
			}
			else {
				break;
			}
		}

		size_t len = 0u;
		if ( c == '\"' || c == '\'' ) {
			// handle quoted strings (both " and ')
			const char quotechar = c;
			data++;
			while ( 1 ) {
				c = *data++;
				if ( c == quotechar || !c || ((c == '\r' || c == '\n') && !allowLineBreaks) ) {
					token[len] = '\0';
					return token;
				}

				if ( len < sizeof(token) ) {
					token[len++] = c;
				}
			}
		}

		// parse a token
		while ( c > 32 ) {
			if ( len < sizeof(token) ) {
				token[len++] = c;
			}
			data++;

			c = *data;
			if ( c == '\n' ) {
				numLines++;
			}
		};

		token[len] = '\0';

		return token;
	}

	bool TokenParser::ParseString( const char **s ) {
		*s = ParseToken( false );
		return (*s == nullptr) ? true : false;
	}

	bool TokenParser::ParseInt32( int32_t *i ) {
		const char *tok = ParseToken( false );

		if ( *tok == '\0' ) {
			return true;
		}

		*i = atoi( tok );
		return false;
	}

	bool TokenParser::ParseUInt32( uint32_t *i ) {
		const char *tok = ParseToken( false );

		*i = strtoul( tok, NULL, 0 );
		return false;
	}

	bool TokenParser::ParseInt16( int16_t *i ) {
		const char *tok = ParseToken( false );

		if ( *tok == '\0' ) {
			return true;
		}

		*i = static_cast<int16_t>( atoi( tok ) );
		return false;
	}

	bool TokenParser::ParseReal32( real32_t *f ) {
		const char *tok = ParseToken( false );

		if ( *tok == '\0' ) {
			return true;
		}

		*f = strtof( tok, nullptr );
		return false;
	}

	bool TokenParser::ParseReal64( real64_t *f ) {
		const char *tok = ParseToken( false );

		if ( *tok == '\0' ) {
			return true;
		}

		*f = strtod( tok, nullptr );
		return false;
	}

	bool TokenParser::ParseUInt8( uint8_t *i ) {
		const char *tok = ParseToken( false );

		if ( *tok == '\0' ) {
			return true;
		}

		*i = static_cast<uint8_t>( atoi( tok ) );
		return false;
	}

} // namespace XS
