#pragma once

#include <tuple>
#include "XSServer/XSEntity.h"

namespace XS {

	class ByteBuffer;

	namespace ServerGame {

		class EntitySphere : public Entity {
		private:

		public:
			EntitySphere();
			EntitySphere( const EntitySphere& ) = delete;
			EntitySphere& operator=( const EntitySphere& ) = delete;

			void Update(
				const real64_t dt
			);

			void Serialise(
				ByteBuffer *buffer
			) const;
		};

	} // namespace ServerGame

} // namespace XS
