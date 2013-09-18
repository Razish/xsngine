#pragma once

#include <string>


#define CONSOLE_BUFFER_SIZE (1024*64)

namespace XS {

	void Print( std::string fmt, ... );

	extern class Console {
	private:
		// text buffer
		//	most significant 8 bits are the colour
		//	least significant 8 bits is the character
		struct consoleText_s {
			union {
				char colour;
				char character;
			} data;
			short raw;
		};

		consoleText_s text[CONSOLE_BUFFER_SIZE];
		char *tail; // points to next free byte

		bool visible;

		short EncodeCharacter( char lastColour, char c );

	public:
		void Display( void );
		void Append( const char *text );
	} console; // class Console

}; // namespace XS
