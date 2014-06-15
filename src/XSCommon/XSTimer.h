#pragma once

#if defined(XS_OS_WINDOWS)
	#include <Windows.h>
#elif defined(XS_OS_LINUX)
	#include <sys/time.h>
#endif

namespace XS {

	#if defined(XS_OS_WINDOWS)
		typedef LARGE_INTEGER TimeVal;
	#elif defined(XS_OS_LINUX)
		typedef struct timeval TimeVal;
	#endif

	class Timer {
	private:
		TimeVal start, stop;

	public:
		Timer();
		enum class Resolution {
			SECONDS,
			MILLISECONDS,
			MICROSECONDS,
		};

		void	Stop( void );
		double	GetTiming( bool restart = false, Resolution resolution = Resolution::MICROSECONDS );
	};
}
