#pragma once

#ifdef _WIN32
	#include <Windows.h>
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
		double	GetTiming( TimerResolution resolution = MILLISECONDS );
	};
}
