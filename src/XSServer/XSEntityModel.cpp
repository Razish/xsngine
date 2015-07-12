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

		void EntityModel::Serialise( ByteBuffer *buffer ) const {
			Entity::Serialise( buffer );
		}

	} // namespace ServerGame

} // namespace XS
