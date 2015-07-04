#pragma once

#include "XSServer/XSEntity.h"

namespace XS {

	class ByteBuffer;

	namespace ServerGame {

		class EntityFXRunner : public Entity {
		private:

		public:
			uint32_t count;
			uint32_t life;

			EntityFXRunner();

			void Update(
				real64_t dt
			);

			void Serialise(
				ByteBuffer *buffer
			) const;
		};

	} // namespace ServerGame

} // namespace XS
