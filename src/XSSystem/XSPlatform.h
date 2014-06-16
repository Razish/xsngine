#pragma once

#define PRODUCT_NAME			"xsngine"
#ifdef _DEBUG
	#define PRODUCT_VERSION		"0.1 DEBUG"
#else
	#define PRODUCT_VERSION		"0.1"
#endif

#ifndef ARCH_STRING
	#error "Unknown architecture"
#endif // ARCH_STRING

#define WINDOW_TITLE			"xsngine [" PRODUCT_VERSION "] " ARCH_STRING

// Win64
#if defined(_WIN64) || defined(__WIN64__)

	#if defined(_MSC_VER)
		#define OS_STRING "win_msvc64"
	#elif defined(__MINGW64__)
		#define OS_STRING "win_mingw64"
	#endif
	#define PATH_SEP '\\'
	#define DLL_EXT ".dll"

	#define XS_LITTLE_ENDIAN
	#define XS_OS_WINDOWS

// Win32
#elif defined(_WIN32) || defined(__WIN32__)

	#if defined(_MSC_VER)
		#define OS_STRING "win_msvc"
	#elif defined(__MINGW32__)
		#define OS_STRING "win_mingw"
	#endif
	#define PATH_SEP '\\'
	#define DLL_EXT ".dll"

	#define XS_LITTLE_ENDIAN
	#define XS_OS_WINDOWS

// MAC OS X
#elif defined(MACOS_X) || defined(__APPLE_CC__)

	#define OS_STRING "macosx"
	#define PATH_SEP '/'
	#define DLL_EXT ".dylib"

	//TODO: endianness?

	#define XS_OS_MAC

// Linux
#elif defined(__linux__)

	#define OS_STRING "linux"
	#define PATH_SEP '/'
	#define DLL_EXT ".so"

	//TODO: endianness?

	//FIXME: String::Compare?
	#define stricmp strcasecmp
	#define strnicmp strncasecmp

	#define XS_OS_LINUX

#endif


// catch missing defines in above blocks
#if !defined(OS_STRING)
	#error "Operating system not supported"
#endif

#if !defined(ARCH_STRING)
	#error "Architecture not supported"
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

// architecture width
#ifndef ARCH_WIDTH
	#if UINTPTR_MAX == 0xffffffff
		#define ARCH_WIDTH 32
	#elif UINTPTR_MAX == 0xffffffffffffffff
		#define ARCH_WIDTH 64
	#else
		#error "Could not determine architecture width"
	#endif
#endif
