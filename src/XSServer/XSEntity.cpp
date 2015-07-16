#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSServer/XSEntity.h"
#include "XSServer/XSServerGame.h"
#include "XSServer/XSServer.h"

namespace XS {

	namespace ServerGame {

		uint32_t Entity::privateNumEntities = 0u;
		const uint32_t &Entity::numEntities = Entity::privateNumEntities;

		// public
		Entity::Entity()
		: id( privateNumEntities++ )
		{
			type = EntityType::Generic;
		}

		Entity::~Entity() {
			state.net.removedEntities.push_back( id );
			privateNumEntities--;
		};

		void Entity::Update( real64_t dt ) {
			position[0] = std::sin( Server::GetElapsedTime() / 250.0 );
			position[2] = std::cos( Server::GetElapsedTime() / 500.0 );
			if ( runPhysics ) {
				//TODO: physics
			}
		}

		void Entity::Serialise( ByteBuffer *buffer ) const {
			// header info
			buffer->WriteUInt32( id );
			buffer->WriteUInt32( type );

			buffer->WriteReal32( position[0] );
			buffer->WriteReal32( position[1] );
			buffer->WriteReal32( position[2] );
		}

	} // namespace ServerGame

} // namespace XS
