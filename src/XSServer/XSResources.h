#pragma once

namespace XS {

	class ByteBuffer;

	namespace ServerGame {

		// ???
		uint32_t GetResourceID(
			const char *name
		);

		// ???
		void Serialise(
			ByteBuffer *buffer
		);

	} // namespace ServerGame

} // namespace XS
