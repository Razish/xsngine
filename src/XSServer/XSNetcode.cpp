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
			buffer->WriteGeneric( &snapshotHeader, sizeof(snapshotHeader) );

			for ( auto &entity : state.entities ) {
				entity->Serialise( buffer );
			}
		}

	} // namespace ServerGame

} // namespace XS
