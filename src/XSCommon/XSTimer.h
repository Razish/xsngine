#pragma once

#if defined(XS_OS_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#ifndef NOMINMAX
		#define NOMINMAX /* Don't define min() and max() */
	#endif
	#include <Windows.h>
#elif defined(XS_OS_LINUX)
	#include <sys/time.h>
#elif defined(XS_OS_MAC)
	#include <sys/time.h>
#endif

namespace XS {

	#if defined(XS_OS_WINDOWS)
		typedef LARGE_INTEGER TimeValue;
	#elif defined(XS_OS_LINUX)
		typedef struct timeval TimeValue;
	#elif defined(XS_OS_MAC)
		typedef struct timeval TimeValue;
	#endif

	enum class TimerResolution {
		Seconds,
		Milliseconds,
		Microseconds,
	};

	class Timer {
	private:
		TimeValue	start;
		TimeValue	stop;

	public:
		Timer();

		// start timing
		void Start(
			void
		);

		// stop timing
		void Stop(
			void
		);

		// get the current elapsed time, optionally resetting the timer to the current time
		real64_t GetTiming(
			bool restart = false,
			TimerResolution resolution = TimerResolution::Microseconds
		) XS_WARN_UNUSED_RESULT;
	};

} // namespace XS
