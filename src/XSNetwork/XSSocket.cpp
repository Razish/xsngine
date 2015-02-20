#include "XSCommon/XSCommon.h"
#include "XSNetwork/XSSocket.h"

namespace XS {

	Socket::Socket( const NetAddress *socketAddress, uint16_t socketPort )
	: port( htons( socketPort ) )
	{
		*address = socketAddress->ToNetworkOrder();
	}

	void Socket::Send( const uint8_t *buf, size_t bufSize ) const {
		// ...
	}

} // namespace XS
