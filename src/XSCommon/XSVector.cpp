#include "XSSystem/XSInclude.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSVector.h"

namespace XS {

	const char *vector2::tostring( void ) const {
		static char buf[64];
		String::FormatBuffer( buf, sizeof( buf ), "%.2f, %.2f", x, y );
		return buf;
	}

	const char *vector3::tostring( void ) const {
		static char buf[64];
		String::FormatBuffer( buf, sizeof( buf ), "%.2f, %.2f, %.2f", x, y, z );
		return buf;
	}

	const char *vector4::tostring( void ) const {
		static char buf[64];
		String::FormatBuffer( buf, sizeof( buf ), "%.2f, %.2f, %.2f, %.2f", x, y, w );
		return buf;
	}

} // namespace XS
