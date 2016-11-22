#pragma once

#include "Common/Vector.h"
#include "Common/ByteBuffer.h"
#include "Shared/EntityTypes.h"
#include "Physics/PhysicsActor.h"

namespace ServerGame {

	class Entity {

	public:
		using ID = uint32_t;

	protected:
		Entity();

	private:
		static ID		privateNumEntities;

	public:
		static const ID	&numEntities;
		static const ID	 invalidID;

		ID			id = invalidID;
		EntityType	type = EntityType::Generic;
		vector3		position;

		struct PhysicsProperties {
			bool			enabled;
			Physics::Actor	*actor;
		} physics = {};

		Entity( const Entity& ) = delete;
		Entity& operator=( const Entity& ) = delete;

		virtual ~Entity();

		virtual void Update(
			real64_t dt
		);

		virtual ByteBuffer::Error Serialise(
			ByteBuffer *buffer
		) const XS_WARN_UNUSED_RESULT;

	};

} // namespace ServerGame
