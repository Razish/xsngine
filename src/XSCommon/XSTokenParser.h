#pragma once

namespace XS {

	class TokenParser {

	private:
		char		token[1024]; // this is returned when parser.ParseXXX is called
		uint32_t	numLines;
		const char	*data;

	public:
		// begin a new parsing session
		TokenParser(
			const char *buffer
		);

		// don't allow default instantiation
		TokenParser() = delete;
		TokenParser( const TokenParser& ) = delete;
		TokenParser& operator=( const TokenParser& ) = delete;

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
		bool ParseString(
			const char **s
		);

		// parse the current token as an unsigned 32 bit integer
		bool ParseUInt(
			uint32_t *i
		);

		// parse the current token as a signed 32 bit integer
		bool ParseInt(
			int32_t *i
		);

		// parse the current token as a signed 32 bit short
		bool ParseShort(
			int16_t *i
		);

		// parse the current token as a floating point number
		bool ParseFloat(
			real32_t *f
		);

		// parse the current token as an unsigned 8 bit integer
		bool ParseByte(
			uint8_t *i
		);

	};

} // namespace XS
