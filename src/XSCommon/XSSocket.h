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

#include "XSCommon/XSNetAddress.h"

namespace XS {

	// helper class
	// creates a socket and connects to the specified host
	//TODO: supports socksv5
	class Socket {
	private:
		enum class Type {
			TCP,
			UDP,
			SOCKS_TCP,
			SOCKS_UDP
		};

		Socket::Type type;
		NetAddress *address;
		short port; // stored in network byte order

	public:
		Socket( const NetAddress *address, short port );

		Socket() = delete;
		Socket( const Socket& ) = delete;
		Socket& operator=( const Socket& ) = delete;

		void	Send	( const byte *buf, size_t bufSize ) const;
	};

} // namespace XS
