#include "XSCommon/XSCommon.h"
#include "XSCommon/XSTokenParser.h"

namespace XS {

	TokenParser::TokenParser( const char *buffer )
	: token{ '\0' }, numLines( 1 ), data( buffer )
	{
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

		uint32_t len = 0u;
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

	bool TokenParser::ParseUInt( unsigned int *i ) {
		const char *tok = ParseToken( false );

		*i = strtoul( tok, NULL, 0 );
		return false;
	}

	bool TokenParser::ParseInt( int *i ) {
		const char *tok = ParseToken( false );

		if ( *tok == '\0' ) {
			return true;
		}

		*i = atoi( tok );
		return false;
	}

	bool TokenParser::ParseShort( short *i ) {
		const char *tok = ParseToken( false );

		if ( *tok == '\0' ) {
			return true;
		}

		*i = static_cast<short>( atoi( tok ) );
		return false;
	}

	bool TokenParser::ParseFloat( float *f ) {
		const char *tok = ParseToken( false );

		if ( *tok == '\0' ) {
			return true;
		}

		*f = static_cast<float>( atof( tok ) );
		return false;
	}

	bool TokenParser::ParseByte( byte *i ) {
		const char *tok = ParseToken( false );

		if ( *tok == '\0' ) {
			return true;
		}

		*i = (byte)atoi( tok );
		return false;
	}

} // namespace XS
