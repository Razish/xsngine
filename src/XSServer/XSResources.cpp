#include <string>
#include <unordered_map>
#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSNetwork/XSNetwork.h"
#include "XSServer/XSResources.h"
#include "XSServer/XSNetcode.h"
#include "XSServer/XSClient.h"

namespace XS {

	namespace ServerGame {

		static std::unordered_map<std::string, uint32_t> resources;
		static uint32_t numResources = 0u;
		static bool modified = true;

		uint32_t GetResourceID( const char *name ) {
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

		static void SerialiseResources( ByteBuffer *buffer ) {
			//TODO: determine actual number of resources, in-case we remove some later..?
			buffer->WriteUInt32( numResources );
			for ( auto &it : resources ) {
				buffer->WriteUInt32( it.second );
				buffer->WriteString( it.first.c_str() );
			}
		}

		//FIXME: selectively network to out-of-date clients (i.e. second client connecting later, not having the list
		//	of resources yet, but `modified` being false.
		//	`force` bool is a hack, perhaps drive the networking further up the chain according to client's GUIDs
		void NetworkResources( bool force ) {
			if ( modified || force ) {
				// send resource list
				ByteBuffer resourceBuffer;
				ServerGame::SerialiseResources( &resourceBuffer );

				Network::XSPacket resourcePacket( Network::ID_XS_SV2CL_RESOURCES );
				resourcePacket.data = resourceBuffer.GetMemory( &resourcePacket.dataLen );

				for ( auto &client : Server::clients ) {
					if ( !client.second ) {
						SDL_assert( !"Invalid client instance - did you instantiate and use before initialising?" );
						continue;
					}
						Network::Send( client.second->guid, &resourcePacket );
				}

				modified = false;
			}
		}

	} // namespace ServerGame

} // namespace XS
