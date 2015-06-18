#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSServer/XSClient.h"
#include "XSServer/XSServer.h"

namespace XS {

	namespace Server {

		Client::Client( uint64_t guid )
		: guid( guid )
		{
			BroadcastMessage( String::Format( "Connection from %X", guid ).c_str() );
		}

	} // namespace ServerGame

} // namespace XS
