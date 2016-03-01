#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSServer/XSEntity.h"
#include "XSServer/XSServerGame.h"
#include "XSServer/XSServer.h"

namespace XS {

	namespace ServerGame {

		Entity::ID Entity::privateNumEntities = 0u;
		const Entity::ID &Entity::numEntities = Entity::privateNumEntities;
		const Entity::ID Entity::invalidID = 0xFFFFFFFFu;

		// public
		//FIXME: have a single CreateEntity function to avoid changing global state on an accidentally-created local
		//	Entity
		Entity::Entity()
		: id( privateNumEntities++ )
		{
			type = EntityType::Generic;
		}

		Entity::~Entity() {
			svgState.net.removedEntities.push_back( id );
			privateNumEntities--;
			delete physics.actor;
		};

		void Entity::Update( real64_t dt ) {
			position[0] = std::sin( Server::GetElapsedTime() / 250.0 );
			position[2] = std::cos( Server::GetElapsedTime() / 500.0 );
			if ( physics.enabled ) {
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
