#pragma once

namespace XS {

	#ifndef REVISION
		#define REVISION "unavailable"
	#endif

	#define XS_MAX_FILENAME (64)

	typedef unsigned char byte; // unsigned 8 bits
	typedef float number; // signed 32 bits
	typedef signed int integer; // signed 32 bits

	// 32 bit field aliasing
	typedef union byteAlias_u {
		float f;
		int32_t i;
		uint32_t ui;
		byte b[4];
		char c[4];
	} byteAlias_t;

	// pointer aliasing
	typedef union pointerAlias_u {
		intptr_t ip;
		void *v;
		float *f;
		int32_t *i;
		uint32_t *ui;
		byte *b;
		char *c;
	} pointerAlias_t;

	#define ARRAY_LEN( x ) (sizeof( (x) ) / sizeof( (*x) ))
	#define STR( str ) #str
	#define XSTR( str ) STR( str )

	#ifndef M_PI
		#define M_PI (3.14159265358979323846f)
	#endif

} // namespace XS
