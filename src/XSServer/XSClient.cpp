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

		Client *GetClient( uint64_t guid ) {
			// linear search
			//FIXME: use guid as key?
			for ( Client *client : clients ) {
				if ( client->guid == guid ) {
					return client;
				}
			}

			return nullptr;
		}

	} // namespace ServerGame

} // namespace XS
