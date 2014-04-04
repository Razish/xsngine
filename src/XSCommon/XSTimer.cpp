#include "XSSystem/XSInclude.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSTimer.h"

// Author: Lourens Elzinga, Kieren McDevitt

namespace XS {

	Timer::Timer() {
	#ifdef _WIN32
		QueryPerformanceCounter( &start );
	#elif defined(__linux__)
		gettimeofday( &start, NULL );
	#endif
	}

	void Timer::Stop( void ) {
	#ifdef _WIN32
		QueryPerformanceCounter( &stop );
	#elif defined(__linux__)
		gettimeofday( &stop, NULL );
	#endif
	}

	double Timer::GetTiming( bool restart, Resolution resolution ) {
		double startTime = 0.0, stopTime = 0.0;

		Stop();

	#ifdef _WIN32
		TimeVal frequency;
		QueryPerformanceFrequency( &frequency );

		startTime = start.QuadPart * (1000000.0 / frequency.QuadPart);
		stopTime = stop.QuadPart * (1000000.0 / frequency.QuadPart);
	#elif defined(__linux__)
		startTime = (start.tv_sec * 1000000.0) + start.tv_usec;
		stopTime = (stop.tv_sec * 1000000.0) + stop.tv_usec;
	#endif

		if ( restart ) {
		#ifdef _WIN32
			QueryPerformanceCounter( &start );
		#elif defined(__linux__)
			gettimeofday( &start, NULL );
		#endif
		}

		switch ( resolution ) {
		case Resolution::SECONDS:
			return (stopTime - startTime) * 0.000001;
		case Resolution::MILLISECONDS:
			return (stopTime - startTime) * 0.001;
		case Resolution::MICROSECONDS:
			return (stopTime - startTime);
		default:
			// should not happen
			return 0.0;
		}
	}

} // namespace XS
