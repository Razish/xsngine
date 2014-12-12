#pragma once

namespace XS {

	class TokenParser {

	private:
		char		token[1024]; // this is returned when parser.ParseXXX is called
		uint32_t	numLines;
		const char	*data;

	public:

		TokenParser( const char *buffer );

		// don't allow default instantiation
		TokenParser() = delete;
		TokenParser( const TokenParser& ) = delete;
		TokenParser& operator=( const TokenParser& ) = delete;

		uint32_t		GetCurrentLine	( void ) const;
		bool			SkipWhitespace	( bool *hasNewLines = nullptr );
		bool			SkipLine		( void );
		const char *	ParseToken		( bool allowLineBreaks = true );
		bool			ParseString		( const char **s );
		bool			ParseUInt		( unsigned int *i );
		bool			ParseInt		( int *i );
		bool			ParseShort		( short *i );
		bool			ParseFloat		( float *f );
		bool			ParseByte		( byte *i );

	};

} // namespace XS
