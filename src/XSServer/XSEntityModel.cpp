#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSServer/XSEntityModel.h"

namespace XS {

	namespace ServerGame {

		EntityModel::EntityModel() {
			type = EntityType::Model;
		}

		void EntityModel::Update( real64_t dt ) {
			Entity::Update( dt );
		}

		ByteBuffer::Error EntityModel::Serialise( ByteBuffer *buffer ) const {
			ByteBuffer::Error status;
			status = Entity::Serialise( buffer );
			status = buffer->Write<uint32_t>( modelIndex );
			if ( status != ByteBuffer::Error::Success ) {
				return status;
			}

			return ByteBuffer::Error::Success;
		}

	} // namespace ServerGame

} // namespace XS
