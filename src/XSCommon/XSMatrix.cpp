#include "XSCommon/XSCommon.h"
#include "XSCommon/XSMatrix.h"

namespace XS {

	std::string matrix4::tostring( void ) const {
		std::string ln = "\n";
		return data[0].tostring() + ln + data[1].tostring() + ln + data[2].tostring() + ln + data[3].tostring();
	}

} // namespace XS
