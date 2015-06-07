#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSServer/XSEntitySphere.h"

namespace XS {

	namespace ServerGame {

		EntitySphere::EntitySphere() {
			console.Print( PrintLevel::Normal,
				"%s\n",
				XS_FUNCTION_VERBOSE
			);
		}

		void EntitySphere::Update( real64_t dt ) {
			// ...
		}

		void EntitySphere::Serialise( ByteBuffer *buffer ) const {
			buffer->WriteString( "EntitySphere" );
		}

	} // namespace ServerGame

} // namespace XS
