#pragma once

#if defined(XS_OS_WINDOWS)

#include <winsock.h>

#elif defined(XS_OS_LINUX)

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#elif defined(XS_OS_MAC)

#include <sys/sockio.h>
#include <net/if.h>
#include <net/if_types.h>
#include <net/if_dl.h>

#endif

#include "XSNetwork/XSNetAddress.h"

namespace XS {

	// helper class
	// creates a socket and connects to the specified host
	//TODO: supports socksv5

	enum class SocketType {
		TCP,
		UDP,
		SOCKS_TCP,
		SOCKS_UDP
	};

	class Socket {
	private:
		//SocketType	 type;
		NetAddress		*address;
		uint16_t		 port; // stored in network byte order

	public:
		// don't allow default instantiation
		Socket() = delete;
		Socket( const Socket& ) = delete;
		Socket& operator=( const Socket& ) = delete;

		Socket(
			const NetAddress *socketAddress,
			uint16_t socketPort
		);

		void Send(
			const uint8_t *buf,
			size_t bufSize
		) const;
	};

} // namespace XS
