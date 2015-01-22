#include "XSCommon/XSCommon.h"
#include "XSNetwork/XSNetAddress.h"

namespace XS {

	NetAddress::NetAddress( const char *hostname )
	: resolved( false ), valid( false )
	{
		// ...
	}

	NetAddress NetAddress::ToNetworkOrder( void ) const {
		//TODO: swap address representation to network byte order
		return *this;
	}

} // namespace XS
