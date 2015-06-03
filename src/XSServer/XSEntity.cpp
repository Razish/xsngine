#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSServer/XSEntity.h"
#include "XSServer/XSServerGame.h"

namespace XS {

	namespace ServerGame {

		// public
		Entity::Entity()
		: id( state.numEntities++ )
		{
			console.Print( PrintLevel::Normal,
				"%s\n",
				XS_FUNCTION_VERBOSE
			);
			state.entities.push_back( this );
		}

		Entity::~Entity() {
			console.Print( PrintLevel::Normal,
				"%s\n",
				XS_FUNCTION_VERBOSE
			);
		};

		void Entity::Update( real64_t dt ) {
			// ...
		}

		uint8_t *Entity::Serialise( size_t *outBufSize ) const {
			if ( outBufSize ) {
				*outBufSize = 0u;
			}
			return nullptr;
		}

	} // namespace ServerGame

} // namespace XS
