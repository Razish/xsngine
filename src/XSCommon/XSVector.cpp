#include "XSSystem/XSInclude.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSVector.h"

namespace XS {

	std::string vector2::tostring( void ) const {
		return String::Format( "%.2f, %.2f", x, y );
	}

	std::string vector3::tostring( void ) const {
		return String::Format( "%.2f, %.2f, %.2f", x, y, z );
	}

	std::string vector4::tostring( void ) const {
		return String::Format( "%.2f, %.2f, %.2f, %.2f", x, y, w );
	}

} // namespace XS
