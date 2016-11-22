#pragma once

#include "XSServer/XSEntity.h"

class ByteBuffer;

namespace ServerGame {

	class EntityFXRunner : public Entity {

	private:
		// ...

	public:
		uint32_t count;
		uint32_t life;

		EntityFXRunner();

		void Update(
			real64_t dt
		);

		ByteBuffer::Error Serialise(
			ByteBuffer *buffer
		) const XS_WARN_UNUSED_RESULT;

	};

} // namespace ServerGame
