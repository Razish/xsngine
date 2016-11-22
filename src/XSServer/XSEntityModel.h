#pragma once

#include <limits>

#include "XSServer/XSEntity.h"

class ByteBuffer;

namespace ServerGame {

	class EntityModel : public Entity {

	public:
		using ModelID = uint32_t;

	private:
		//TODO: invalidID/invalidIndex
		ModelID		modelIndex = std::numeric_limits<ModelID>::max();
		std::string	modelPath;

	public:
		EntityModel();

		void Update(
			real64_t dt
		);

		ByteBuffer::Error Serialise(
			ByteBuffer *buffer
		) const XS_WARN_UNUSED_RESULT;

	};

} // namespace ServerGame
