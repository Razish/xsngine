#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSServer/XSEntityFXRunner.h"

namespace XS {

	namespace ServerGame {

		EntityFXRunner::EntityFXRunner() {
			type = EntityType::FXRunner;
			count = 10u;
			life = 1000u;
		}

		void EntityFXRunner::Update( real64_t dt ) {
			// ...
		}

		void EntityFXRunner::Serialise( ByteBuffer *buffer ) const {
			Entity::Serialise( buffer );
			buffer->WriteUInt32( count );
			buffer->WriteUInt32( life );
		}

	} // namespace ServerGame

} // namespace XS
