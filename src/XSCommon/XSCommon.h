#pragma once

// Responsible for defining common data types and including common library headers

#include <cerrno>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>

#include <algorithm>
#include <string>
#include <vector>

#include <SDL2/SDL_assert.h>

#include "XSSystem/XSPlatform.h"
#include "XSCommon/XSError.h"

namespace XS {

	#ifndef REVISION
		#define REVISION "unavailable"
	#endif

	#define XS_MAX_FILENAME (96)

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
		#define M_PI (3.14159265358979323846)
	#endif

} // namespace XS
