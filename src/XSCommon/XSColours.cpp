#include "XSCommon/XSCommon.h"
#include "XSCommon/XSColours.h"

/*
 1	0	black
 2	1	grey
 3	2	white
 4	3	red
 5	4	orange
 6	5	yellow
 7	6	lime
 8	7	green
 9	8	aqua
10	9	cyan
11	a	light blue
12	b	blue
13	c	violet
14	d	purple
15	e	pink
16	f	*white, to be changed
*/

const vector4 colourTable[COLOUR_BITS + 1] = {
	{ 0.0f, 0.0f, 0.0f, 1.0f }, // black
	{ 0.5f, 0.5f, 0.5f, 1.0f }, // grey
	{ 1.0f, 1.0f, 1.0f, 1.0f }, // white
	{ 1.0f, 0.0f, 0.0f, 1.0f }, // red
	{ 1.0f, 0.5f, 0.0f, 1.0f }, // orange
	{ 1.0f, 1.0f, 0.0f, 1.0f }, // yellow
	{ 0.5f, 1.0f, 0.0f, 1.0f }, // lime
	{ 0.0f, 1.0f, 0.0f, 1.0f }, // green
	{ 0.0f, 1.0f, 0.5f, 1.0f }, // aqua
	{ 0.0f, 1.0f, 1.0f, 1.0f }, // cyan
	{ 0.0f, 0.5f, 1.0f, 1.0f }, // light blue
	{ 0.0f, 0.0f, 1.0f, 1.0f }, // blue
	{ 0.5f, 0.0f, 1.0f, 1.0f }, // violet
	{ 1.0f, 0.0f, 1.0f, 1.0f }, // purple
	{ 1.0f, 0.0f, 0.5f, 1.0f }, // pink
	{ 1.0f, 1.0f, 1.0f, 1.0f }, // *white
};

bool IsColourString( const char *s ) {
	if ( s && s[0] && s[1] && s[0] == COLOUR_ESCAPE ) {
		int lowc = tolower( s[1] );
		if ( (s[1] >= '0' && s[1] <= '9') || (lowc >= 'a' && lowc <= 'e') ) {
			return true;
		}
	}

	return false;
}

size_t ColourStringLength( const char *text ) {
	size_t len = 0;

	if ( !text ) {
		return 0;
	}

	for ( const char *p = text; *p; p++ ) {
		if ( IsColourString( p ) ) {
			p++;
			continue;
		}

		if ( p[0] == COLOUR_ESCAPE && p[1] == COLOUR_ESCAPE ) {
			p++;
		}

		len++;
	}

	return len;
}

uint32_t ColourIndex( char c ) {
	int lowc = tolower( c );

	if ( c >= '0' && c <= '9' ) {
		return (c - '0') & COLOUR_BITS;
	}
	else if ( lowc >= 'a' && lowc <= 'e' ) {
		return (lowc - 'a' + '9') & COLOUR_BITS;
	}

	// default to pink, it's not worth throwing an error
	return ColourIndex( COLOUR_PINK );
}
