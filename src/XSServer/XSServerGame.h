#pragma once

namespace XS {

	namespace ServerGame {

		extern struct GameState {
			uint32_t dummy;
		} state;

		// initialise the ServerGame
		void Init(
			void
		);

		// run a frame, simulate entities
		void RunFrame(
			real64_t dt
		);

	} // namespace ServerGame

} // namespace XS
