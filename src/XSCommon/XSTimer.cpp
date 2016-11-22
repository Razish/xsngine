#include "XSCommon/XSCommon.h"
#include "XSCommon/XSTimer.h"

// Author: Lourens Elzinga, Kieren McDevitt
//TODO: move to platform specific code.

Timer::Timer() {
	Start();
}

void Timer::Start( void ) {
#if defined(XS_OS_WINDOWS)
	QueryPerformanceCounter( &start );
#elif defined(XS_OS_LINUX) || defined(XS_OS_MAC)
	gettimeofday( &start, nullptr );
#endif
}

void Timer::Stop( void ) {
#if defined(XS_OS_WINDOWS)
	QueryPerformanceCounter( &stop );
#elif defined(XS_OS_LINUX) || defined(XS_OS_MAC)
	gettimeofday( &stop, nullptr );
#endif
}

real64_t Timer::GetTiming( bool restart, TimerResolution resolution ) {
	Stop();

#if defined(XS_OS_WINDOWS)
	TimeValue frequency;
	QueryPerformanceFrequency( &frequency );

	real64_t startTime = start.QuadPart * (1000000.0 / frequency.QuadPart);
	real64_t stopTime = stop.QuadPart * (1000000.0 / frequency.QuadPart);
#elif defined(XS_OS_LINUX) || defined(XS_OS_MAC)
	real64_t startTime = (start.tv_sec * 1000000.0) + start.tv_usec;
	real64_t stopTime = (stop.tv_sec * 1000000.0) + stop.tv_usec;
#endif

	if ( restart ) {
		Start();
	}

	switch ( resolution ) {
	case TimerResolution::Seconds: {
		return (stopTime - startTime) * 0.000001;
	} break;
	case TimerResolution::Milliseconds: {
		return (stopTime - startTime) * 0.001;
	} break;
	case TimerResolution::Microseconds: {
		return (stopTime - startTime);
	} break;
	default: {
		SDL_assert( !"Invalid timer resolution" );
		return 0.0;
	} break;
	}
}
