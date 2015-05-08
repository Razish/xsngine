#pragma once

// Responsible for defining common data types and including common library headers

#include <cerrno>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <SDL2/SDL_assert.h>

#include "XSSystem/XSPlatform.h"

//TODO: determine max filepath for current filesystem at runtime (NTFS, ext4, HFS+)
#define XS_MAX_FILENAME (96)

typedef float real32_t;
typedef double real64_t;
typedef long double real80_t;

// 32 bit field aliasing
union byteAlias32_t {
	real32_t	f32;
	int32_t		i32;
	uint32_t	ui32;
	int8_t		i8[4];
	uint8_t		ui8[4];
};

// pointer aliasing
union pointerAlias_t {
	intptr_t	iptr;
	void		*v;
	real32_t	*f32;
	real64_t	*f64;
	int32_t		*i32;
	uint32_t	*ui32;
	uint8_t		*ui8;
	int8_t		*i8;
};

#define ARRAY_LEN( x ) (sizeof((x)) / sizeof((*x)))
#define STR( str ) #str
#define XSTR( str ) STR( str )

#ifndef M_PI
	#define M_PI (3.14159265358979323846)
#endif
