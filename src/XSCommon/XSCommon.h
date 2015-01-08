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

namespace XS {

	#ifndef REVISION
		#define REVISION "unavailable"
	#endif

	//TODO: determine max filepath for current filesystem at runtime (NTFS, ext4, HFS+)
	#define XS_MAX_FILENAME (96)

	// 32 bit field aliasing
	union byteAlias32_t {
		float f32;
		int32_t i32;
		uint32_t ui32;
		int8_t i8[4];
		uint8_t ui8[4];
	};

	// pointer aliasing
	union pointerAlias_t {
		intptr_t iptr;
		void *v;
		float *f32;
		int32_t *i32;
		uint32_t *ui32;
		uint8_t *ui8;
		int8_t *i8;
	};

	#define ARRAY_LEN( x ) (sizeof( (x) ) / sizeof( (*x) ))
	#define STR( str ) #str
	#define XSTR( str ) STR( str )

	#ifndef M_PI
		#define M_PI (3.14159265358979323846)
	#endif

} // namespace XS
