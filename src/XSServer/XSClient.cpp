#include "XSCommon/XSCommon.h"
#include "XSServer/XSClient.h"
#include "XSServer/XSServer.h"

namespace XS {

	namespace Server {

		Client::Client( uint64_t guid )
		: guid( guid )
		{
			BroadcastMessage( "Connection from %s\n" );
		}

	} // namespace ServerGame

} // namespace XS
