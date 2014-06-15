#pragma once

#include "XSCommon/XSVector.h"

namespace XS {

	#define COLOUR_ESCAPE	'^'
	#define COLOUR_BITS		(0xF)

	#define COLOUR_BLACK		'0'
	#define COLOUR_GREY			'1'
	#define COLOUR_WHITE		'2'
	#define COLOUR_RED			'3'
	#define COLOUR_ORANGE		'4'
	#define COLOUR_YELLOW		'5'
	#define COLOUR_LIME			'6'
	#define COLOUR_GREEN		'7'
	#define COLOUR_AQUA			'8'
	#define COLOUR_CYAN			'9'
	#define COLOUR_LIGHTBLUE	'a'
	#define COLOUR_BLUE			'b'
	#define COLOUR_VIOLET		'c'
	#define COLOUR_PURPLE		'd'
	#define COLOUR_PINK			'e'

	#define S_COLOUR_BLACK		"^0"
	#define S_COLOUR_GREY		"^1"
	#define S_COLOUR_WHITE		"^2"
	#define S_COLOUR_RED		"^3"
	#define S_COLOUR_ORANGE		"^4"
	#define S_COLOUR_YELLOW		"^5"
	#define S_COLOUR_LIME		"^6"
	#define S_COLOUR_GREEN		"^7"
	#define S_COLOUR_AQUA		"^8"
	#define S_COLOUR_CYAN		"^9"
	#define S_COLOUR_LIGHTBLUE	"^a"
	#define S_COLOUR_BLUE		"^b"
	#define S_COLOUR_VIOLET		"^c"
	#define S_COLOUR_PURPLE		"^d"
	#define S_COLOUR_PINK		"^e"

	bool IsColourString( const char *p );
	size_t ColourStringLength( const char *text );
	unsigned int ColourIndex( char c );
	extern const vector4 g_colour_table[COLOUR_BITS+1];

} // namespace XS
