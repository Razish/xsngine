#include "Common/Common.h"
#include "Common/Console.h"
#include "Common/ByteBuffer.h"
#include "Server/Entity.h"
#include "Server/ServerGame.h"
#include "Server/Server.h"

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

	ByteBuffer::Error Entity::Serialise( ByteBuffer *buffer ) const {
		ByteBuffer::Error status;
		// header info
		status = buffer->Write<uint32_t>( id );
		status = buffer->Write<uint32_t>( type );
		if ( status != ByteBuffer::Error::Success ) {
			return status;
		}

		status = buffer->Write<uint32_t>( position[0] );
		status = buffer->Write<uint32_t>( position[1] );
		status = buffer->Write<uint32_t>( position[2] );
		if ( status != ByteBuffer::Error::Success ) {
			return status;
		}

		return ByteBuffer::Error::Success;
	}

} // namespace ServerGame
