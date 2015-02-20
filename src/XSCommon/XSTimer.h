#pragma once

#if defined(XS_OS_WINDOWS)
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
		);
	};

} // namespace XS
