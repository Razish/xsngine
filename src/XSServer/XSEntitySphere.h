#pragma once

#include <tuple>
#include "XSServer/XSEntity.h"

namespace XS {

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

			uint8_t *Serialise(
				size_t *outBufSize
			) const;
		};

	} // namespace ServerGame

} // namespace XS
