#pragma once

#include <string>
#include <vector>

#include "XSCommon/XSVector.h"

namespace XS {

	namespace ServerGame {

		class Entity {
		protected:
			Entity();

		public:
			uint32_t						id;
			vector3							position;

			Entity( const Entity& ) = delete;
			Entity& operator=( const Entity& ) = delete;

			virtual ~Entity();

			virtual void Update(
				const real64_t dt
			);

			virtual uint8_t *Serialise(
				size_t *outBufSize
			) const;
		};

	} // namespace ServerGame

} // namespace XS
