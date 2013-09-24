#pragma once

namespace XS {

	#define XS_MAX_FILENAME (64)

	typedef unsigned char	byte;		// unsigned  8 bits
	typedef float			number;		//   signed 32 bits
	typedef signed int		integer;	//   signed 32 bits

	#define ARRAY_LEN( x ) (sizeof( (x) ) / sizeof( (*x) ))

} // namespace XS
