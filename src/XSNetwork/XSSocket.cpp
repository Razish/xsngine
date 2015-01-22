#include "XSCommon/XSCommon.h"
#include "XSNetwork/XSSocket.h"

namespace XS {

	Socket::Socket( const NetAddress *address, short port ) {
		this->port = htons( port );
		(*this->address) = address->ToNetworkOrder();
	}

	void Socket::Send( const uint8_t *buf, size_t bufSize ) const {
		// ...
	}

} // namespace XS
