#pragma once

#ifndef XS_REVISION
	#define XS_REVISION "unavailable"
#endif

#if defined(_DEBUG)
	#define PRODUCT_VERSION "development debug - rev " XS_REVISION
#else
	#define PRODUCT_VERSION "development - rev " XS_REVISION
#endif

#ifndef ARCH_STRING
	#error "Unknown architecture"
#endif // ARCH_STRING

#define WINDOW_TITLE PRODUCT_NAME " [" PRODUCT_VERSION "] " ARCH_STRING

// Win64
#if defined(_WIN64) || defined(__WIN64__)
	#if defined(_MSC_VER)
		#define OS_STRING "win_msvc64"
	#elif defined(__MINGW64__)
		#define OS_STRING "win_mingw64"
	#endif
	#define PATH_SEP '\\'
	#define DLL_EXT ".dll"

	// Visual Studio <= 1800 doesn't support noexcept, throw() has the closest behaviour
	#if defined(_MSC_VER) && _MSC_VER <= 1800
		#define noexcept throw()
	#endif

	#define XS_LITTLE_ENDIAN

// Win32
#elif defined(_WIN32) || defined(__WIN32__)

	#if defined(_MSC_VER)
		#define OS_STRING "win_msvc"
	#elif defined(__MINGW32__)
		#define OS_STRING "win_mingw"
	#endif
	#define PATH_SEP '\\'
	#define DLL_EXT ".dll"

	// Visual Studio still doesn't support noexcept
	#if defined(_MSC_VER) && _MSC_VER <= 1800
		#define noexcept throw()
	#endif

	#define XS_LITTLE_ENDIAN

// MAC OS X
#elif defined(MACOS_X) || defined(__APPLE_CC__)

	#define OS_STRING "macosx"
	#define PATH_SEP '/'
	#define DLL_EXT ".dylib"

	//TODO: endianness?

// Linux
#elif defined(__linux__)

	#define OS_STRING "linux"
	#define PATH_SEP '/'
	#define DLL_EXT ".so"

	//TODO: endianness?

	//FIXME: String::Compare?
	#define stricmp strcasecmp
	#define strnicmp strncasecmp

#endif


// catch missing defines in above blocks
#if !defined(OS_STRING)
	#error "Operating system not supported"
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


// compiler specific junk, function names etc
#if defined(_MSC_VER)

	// visual studio
	#define XS_FUNCTION __FUNCTION__
	#define XS_FUNCTION_VERBOSE __FUNCSIG__
	#define XS_EXPORT __declspec(dllexport)
	#define XS_NAKED __declspec(naked)
	#define XS_USED
	#define XS_UNUSED
	#define XS_WARN_UNUSED_RESULT
	#define XS_WARN_DEPRECATED
	#define XS_CDECL __cdecl
	#define XS_PURE
	#define XS_PERF_HOT
	#define XS_PERF_COLD

#elif defined(__GNUC__) || defined(__clang__)

	// gcc, clang
	#define XS_FUNCTION __FUNCTION__
	#define XS_FUNCTION_VERBOSE __PRETTY_FUNCTION__
	#define XS_EXPORT __attribute__(( visibility( "default" ) ))
	#define XS_NAKED __attribute__(( noinline ))
	#define XS_USED __attribute__(( used ))
	#define XS_UNUSED __attribute__(( unused ))
	#define XS_WARN_UNUSED_RESULT __attribute__(( warn_unused_result ))
	#define XS_WARN_DEPRECATED __attribute__(( deprecated ))
	#if defined(XSARCH_X86)
		#define XS_CDECL __attribute__(( cdecl ))
	#else
		#define XS_CDECL
	#endif
	#define XS_PURE __attribute__(( pure ))
	#define XS_PERF_HOT __attribute__(( hot ))
	#define XS_PERF_COLD __attribute__(( cold ))

#elif defined(__INTEL_COMPILER)

	//TODO: icc / intel

#else

	#define XS_FUNCTION "<unknown-func>"
	#define XS_FUNCTION_VERBOSE XS_FUNCTION
	#define XS_EXPORT
	#define XS_NAKED
	#define XS_USED
	#define XS_UNUSED
	#define XS_WARN_UNUSED_RESULT
	#define XS_WARN_DEPRECATED
	#define XS_CDECL
	#define XS_PURE
	#define XS_PERF_HOT
	#define XS_PERF_COLD

#endif

#define XS_CABI extern "C"
