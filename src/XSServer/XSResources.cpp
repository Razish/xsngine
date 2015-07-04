#include <string>
#include <unordered_map>
#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSServer/XSResources.h"
#include "XSServer/XSNetcode.h"

namespace XS {

	namespace ServerGame {

		static std::unordered_map<std::string, uint32_t> resources;
		static uint32_t numResources = 0u;

		uint32_t GetResourceID( const char *name ) {
			if ( resources.find( name ) != resources.end() ) {
				// already exists
				return resources[name];
			}
			else {
				// new resource handle
				resources[name] = numResources;
				return numResources++;
			}
		}

		void Serialise( ByteBuffer *buffer ) {
			//TODO: determine actual number of resources, in-case we remove some later..?
			buffer->WriteUInt32( numResources );
			for ( auto &it : resources ) {
				buffer->WriteUInt32( it.second );
				buffer->WriteString( it.first.c_str() );
			}
		}

	} // namespace ServerGame

} // namespace XS
