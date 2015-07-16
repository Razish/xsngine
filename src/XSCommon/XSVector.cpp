#include "XSCommon/XSCommon.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSString.h"

namespace XS {

	std::string vector2::tostring( void ) const {
		return String::Format( "%.2f, %.2f", data[0], data[1] );
	}

	std::string vector3::tostring( void ) const {
		return String::Format( "%.2f, %.2f, %.2f", data[0], data[1], data[2] );
	}

	std::string pvector3::tostring( void ) const {
		return String::Format( "%.2f, %.2f, %.2f", data[0], data[1], data[2] );
	}

	std::string vector4::tostring( void ) const {
		return String::Format( "%.2f, %.2f, %.2f, %.2f", data[0], data[1], data[2], data[3] );
	}

} // namespace XS
