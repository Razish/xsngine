#include "XSCommon/XSCommon.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSServer/XSNetcode.h"
#include "XSServer/XSEntity.h"

namespace XS {

	namespace ServerGame {

		NetworkState net = {};

		void GenerateSnapshot( ByteBuffer *buffer ) {
			//FIXME: delta etc, this is temporary

			struct SnapshotHeader {
				uint32_t numEntities;
			} snapshotHeader;
			snapshotHeader.numEntities = Entity::numEntities;
			buffer->WriteGeneric( &snapshotHeader, sizeof(snapshotHeader) );

			for ( auto &entity : state.entities ) {
				entity->Serialise( buffer );
			}
		}

	} // namespace ServerGame

} // namespace XS
