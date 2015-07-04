#pragma once

#include "XSServer/XSEntity.h"

namespace XS {

	class ByteBuffer;

	namespace ServerGame {

		class EntityModel : public Entity {
		private:

		public:
			EntityModel();

			void Update(
				real64_t dt
			);

			void Serialise(
				ByteBuffer *buffer
			) const;
		};

	} // namespace ServerGame

} // namespace XS
