#include <string>
#include <unordered_map>
#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSNetwork/XSNetwork.h"
#include "XSServer/XSResources.h"
#include "XSServer/XSNetcode.h"
#include "XSServer/XSClient.h"

namespace ServerGame {

	static std::unordered_map<std::string, ResourceID> resources;
	static uint32_t numResources = 0u;
	static bool modified = true;

	ResourceID GetResourceID( const char *name ) {
		auto found = resources.find( name );
		if ( found != resources.end() ) {
			// already exists
			return found->second;
		}
		else {
			// new resource handle
			resources[name] = numResources;
			modified = true;
			return numResources++;
		}
	}

	static ByteBuffer::Error SerialiseResources( ByteBuffer *buffer ) {
		ByteBuffer::Error status;
		//TODO: determine actual number of resources, in-case we remove some later..?
		status = buffer->Write<uint32_t>( numResources );
		if ( status != ByteBuffer::Error::Success ) {
			return status;
		}

		for ( auto &it : resources ) {
			status = buffer->Write<uint32_t>( it.second );
			status = buffer->WriteString( it.first.c_str() );
		}
		if ( status != ByteBuffer::Error::Success ) {
			return status;
		}

		return ByteBuffer::Error::Success;
	}

	//FIXME: selectively network to out-of-date clients (i.e. second client connecting later, not having the list
	//	of resources yet, but `modified` being false.
	//	`force` bool is a hack, perhaps drive the networking further up the chain according to client's GUIDs
	void NetworkResources( bool force ) {
		if ( modified || force ) {
			// send resource list
			ByteBuffer resourceBuffer;
			ByteBuffer::Error status;
			status = ServerGame::SerialiseResources( &resourceBuffer );
			if ( (status = ServerGame::SerialiseResources( &resourceBuffer )) != ByteBuffer::Error::Success ) {
				SDL_assert( !"Could not serialise ServerGame instance" );
				return;
			}

			Network::XSPacket resourcePacket( Network::ID_XS_SV2CL_RESOURCES );
			resourcePacket.data = resourceBuffer.GetMemory( &resourcePacket.dataLen );

			for ( auto &client : Server::clients ) {
				if ( !client.second ) {
					SDL_assert(
						!"Invalid client instance on network pump - did you instantiate and use before "
							"initialising?"
					);
					continue;
				}
				client.second->connection.Send( resourcePacket );
			}

			modified = false;
		}
	}

} // namespace ServerGame
