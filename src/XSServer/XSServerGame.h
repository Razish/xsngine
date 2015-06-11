#pragma once

#include <vector>

namespace XS {

	namespace ServerGame {

		extern struct GameState {
			uint32_t	dummy;
		} state;

		// initialise the ServerGame
		void Init(
			void
		);

		// run a frame
		void RunFrame(
			real64_t dt
		);

		// ???
		void Connect(
			const char *ip,
			uint64_t guid
		);

	} // namespace ServerGame

} // namespace XS
