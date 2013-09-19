#pragma once

#define PRODUCT_NAME			"xsngine"
#define PRODUCT_VERSION			"0.1"
#define WINDOW_TITLE			"xsngine [" PRODUCT_VERSION "] " ARCH_STRING


#define XSDECL
#define XSCALL

// Win64
#if defined(_WIN64) || defined(__WIN64__)

	#undef XSDECL
	#define XSDECL __cdecl

	#undef XSCALL
	#define XSCALL __stdcall

	#if defined(_MSC_VER)
		#define OS_STRING "win_msvc64"
	#elif defined(__MINGW64__)
		#define OS_STRING "win_mingw64"
	#endif

	#define XSINLINE __inline
	#define PATH_SEP '\\'

	#define ARCH_STRING "x64"

	#define Q3_LITTLE_ENDIAN

	#define DLL_EXT ".dll"

// Win32
#elif defined(_WIN32) || defined(__WIN32__)

	#undef XSDECL
	#define XSDECL __cdecl

	#undef XSCALL
	#define XSCALL __stdcall

	#if defined(_MSC_VER)
		#define OS_STRING "win_msvc"
	#elif defined(__MINGW32__)
		#define OS_STRING "win_mingw"
	#endif

	#define XSINLINE __inline
	#define PATH_SEP '\\'

	#if defined(_M_IX86) || defined(__i386__)
		#define ARCH_STRING "x86"
	#endif

	#define XS_LITTLE_ENDIAN

	#define DLL_EXT ".dll"

// MAC OS X
#elif defined(MACOS_X) || defined(__APPLE_CC__)

	#define OS_STRING "macosx"
	#define XSINLINE inline
	#define PATH_SEP '/'

	#if defined(__ppc__)
		#define ARCH_STRING "ppc"
	#elif defined(__i386__)
		#define ARCH_STRING "x86"
	#elif defined(__x86_64__)
		#define ARCH_STRING "x86_64"
	#endif

	#define DLL_EXT ".dylib"

// Linux
#elif defined(__linux__)

	#define OS_STRING "linux"

	#ifdef __clang__
		#define XSINLINE static inline
	#else
		#define XSINLINE inline
	#endif

	#define PATH_SEP '/'

	#if defined(__i386__)
		#define ARCH_STRING "x86"
	#elif defined(__x86_64__)
		#define ARCH_STRING "x86_64"
	#elif defined(__ia64__)
		#define ARCH_STRING "ia64"
	#elif defined(__arm__)
		#define ARCH_STRING "arm"
	#elif defined(__mips__)
		#define ARCH_STRING "mips"
	#endif

	#define DLL_EXT ".so"

#endif


// catch missing defines in above blocks
#if !defined(OS_STRING)
	#error "Operating system not supported"
#endif

#if !defined(ARCH_STRING)
	#error "Architecture not supported"
#endif

#if !defined(XSINLINE)
	#error "XSINLINE not defined"
#endif

#if !defined(PATH_SEP)
	#error "PATH_SEP not defined"
#endif

#if !defined(DLL_EXT)
	#error "DLL_EXT not defined"
#endif


// platform string
#if defined(NDEBUG)
	#define PLATFORM_STRING OS_STRING "-" ARCH_STRING
#else
	#define PLATFORM_STRING OS_STRING "-" ARCH_STRING "-debug"
#endif
