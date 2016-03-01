#pragma once

#include "XSCommon/XSVector.h"
#include "XSShared/XSEntityTypes.h"
#include "XSPhysics/XSPhysicsActor.h"

namespace XS {

	class ByteBuffer;

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

			virtual void Serialise(
				ByteBuffer *buffer
			) const;

		};

	} // namespace ServerGame

} // namespace XS
