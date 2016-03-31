#include "XSCommon/XSCommon.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSServer/XSNetcode.h"
#include "XSServer/XSEntity.h"

namespace XS {

	namespace ServerGame {

		NetworkState net = {};

		void GenerateSnapshot( ByteBuffer *buffer ) {
			ByteBuffer::Error status;
			//FIXME: delta etc, this is temporary
			//TODO: bit-pack based on last ACKed state (states should have an accumulative id)

			struct SnapshotHeader {
				//uint32_t sequence;
				uint32_t numEntities;
			} snapshotHeader = {
				Entity::numEntities
			};
			status = buffer->WriteRaw( &snapshotHeader, sizeof(snapshotHeader) );

			for ( auto &entity : svgState.entities ) {
				status = entity->Serialise( buffer );
			}

			//TODO: if debugServerNetworking
			// print( status )
		}

	} // namespace ServerGame

} // namespace XS
