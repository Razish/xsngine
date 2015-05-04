#pragma once

namespace XS {

	class TokenParser {

	private:
		char		token[1024]; // this is returned when parser.ParseXXX is called
		uint32_t	numLines;
		const char	*data;

	public:
		// don't allow default instantiation
		TokenParser() = delete;
		TokenParser( const TokenParser& ) = delete;
		TokenParser& operator=( const TokenParser& ) = delete;

		// begin a new parsing session
		TokenParser(
			const char *buffer
		);

		// get the current line number
		uint32_t GetCurrentLine(
			void
		) const;

		// skip all whitespace until the next token
		bool SkipWhitespace(
			bool *hasNewLines = nullptr
		);

		// skip to the next line
		bool SkipLine(
			void
		);

		// parse the current token
		const char *ParseToken(
			bool allowLineBreaks = true
		);

		// parse the current token as a string
		// returns true upon error
		bool ParseString(
			const char **s
		);

		// parse the current token as a signed 32 bit integer
		// returns true upon error
		bool ParseInt32(
			int32_t *i
		);

		// parse the current token as an unsigned 32 bit integer
		// returns true upon error
		bool ParseUInt32(
			uint32_t *i
		);

		// parse the current token as a signed 16 bit integer
		// returns true upon error
		bool ParseInt16(
			int16_t *i
		);

		// parse the current token as a 32 bit floating point number
		// returns true upon error
		bool ParseReal32(
			real32_t *f
		);

		// parse the current token as a 64 bit floating point number
		// returns true upon error
		bool ParseReal64(
			real64_t *f
		);

		// parse the current token as an unsigned 8 bit integer
		// returns true upon error
		bool ParseUInt8(
			uint8_t *i
		);

	};

} // namespace XS
