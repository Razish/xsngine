#pragma once

#include "XSCommon/XSVector.h"
#include "XSShared/XSEntityTypes.h"

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

			uint32_t	id = 0xFFFFFFFFu;
			EntityType	type = EntityType::Generic;
			vector3		position;
			uint32_t	modelIndex = 0xFFFFFFFFu;
			bool		runPhysics = false;

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
