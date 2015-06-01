#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
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

		uint8_t *EntitySphere::Serialise( size_t *outBufSize ) const {
			if ( outBufSize ) {
				*outBufSize = 0u;
			}
			return nullptr;
		}

	} // namespace ServerGame

} // namespace XS
