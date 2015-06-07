#pragma once

#include "XSCommon/XSVector.h"

namespace XS {

	class ByteBuffer;

	namespace ServerGame {

		class Entity {
		protected:
			Entity();

		public:
			uint32_t	id;
			vector3		position;

			Entity( const Entity& ) = delete;
			Entity& operator=( const Entity& ) = delete;

			virtual ~Entity();

			virtual void Update(
				const real64_t dt
			);

			virtual void Serialise(
				ByteBuffer *buffer
			) const;
		};

	} // namespace ServerGame

} // namespace XS
