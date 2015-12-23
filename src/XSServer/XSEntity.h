#pragma once

#include "XSCommon/XSVector.h"
#include "XSShared/XSEntityTypes.h"
#include "XSPhysics/XSPhysicsActor.h"

namespace XS {

	class ByteBuffer;

	namespace ServerGame {

		class Entity {

		private:
			static uint32_t		privateNumEntities;

		protected:
			Entity();

		public:
			static const uint32_t	&numEntities;
			static const uint32_t	 invalidID;

			uint32_t	id = invalidID;
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

			virtual void Serialise(
				ByteBuffer *buffer
			) const;

		};

	} // namespace ServerGame

} // namespace XS
