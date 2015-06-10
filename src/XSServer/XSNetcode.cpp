#include "XSCommon/XSCommon.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSServer/XSNetcode.h"

namespace XS {

	namespace ServerGame {

		NetworkState net = {};

		void GenerateSnapshot( ByteBuffer *buffer ) {
			//FIXME: delta etc, this is temporary

			struct SnapshotHeader {
				uint32_t dummy = 1337u;
			} snapshotHeader;
			buffer->WriteGeneric( &snapshotHeader, sizeof(snapshotHeader) );
		}

	} // namespace ServerGame

} // namespace XS
