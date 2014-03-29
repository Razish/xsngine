#pragma once

#ifdef _WIN32
	#include <Windows.h>
#elif defined(__linux__)
	#include <sys/time.h>
#endif

namespace XS {

	#ifdef _WIN32
		typedef LARGE_INTEGER TimeVal;
	#elif defined(__linux__)
		typedef struct timeval TimeVal;
	#endif

	class Timer {
	private:
		TimeVal start, stop;

	public:
		Timer();
		enum TimerResolution {
			SECONDS,
			MILLISECONDS,
			MICROSECONDS,
		};

		void	Stop( void );
		double	GetTiming( bool restart = false, TimerResolution resolution = MILLISECONDS );
	};
}
