#include "XSCommon/XSCommon.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSServer/XSNetcode.h"
#include "XSServer/XSEntity.h"

namespace XS {

	namespace ServerGame {

		NetworkState net = {};

		void GenerateSnapshot( ByteBuffer *buffer ) {
			//FIXME: delta etc, this is temporary
			//TODO: bit-pack based on last ACKed state (states should have an accumulative id)

			struct SnapshotHeader {
				//uint32_t sequence;
				uint32_t numEntities;
			} snapshotHeader = {
				Entity::numEntities
			};
			if ( buffer->WriteRaw( &snapshotHeader, sizeof(snapshotHeader) ) == ByteBuffer::Error::Success ) {
				for ( auto &entity : svgState.entities ) {
					if ( entity->Serialise( buffer ) != ByteBuffer::Error::Success ) {
						// ...
					}
				}
			}

			//TODO: if debugServerNetworking
			// print( status )
		}

	} // namespace ServerGame

} // namespace XS
