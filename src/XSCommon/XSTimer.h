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
		typedef LARGE_INTEGER timeVal_t;
	#elif defined(XS_OS_LINUX)
		typedef struct timeval timeVal_t;
	#elif defined(XS_OS_MAC)
		typedef struct timeval timeVal_t;
	#endif

	class Timer {
	private:
		timeVal_t start, stop;

	public:
		enum class Resolution {
			SECONDS,
			MILLISECONDS,
			MICROSECONDS,
		};

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
			Resolution resolution = Resolution::MICROSECONDS
		);
	};

} // namespace XS
