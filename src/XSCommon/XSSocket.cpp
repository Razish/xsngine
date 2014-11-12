#include "XSCommon/XSCommon.h"
#include "XSCommon/XSSocket.h"

namespace XS {

	Socket::Socket( const NetAddress *address, short port ) {
		this->port = htons( port );
		(*this->address) = address->ToNetworkOrder();
	}

	void Socket::Send( const byte *buf, size_t bufSize ) const {
		// ...
	}

} // namespace XS
