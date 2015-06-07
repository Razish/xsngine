#pragma once

#include <vector>

namespace XS {

	namespace ServerGame {

		class Entity;

		extern struct GameState {
			std::vector<Entity *>		entities;
			uint32_t					numEntities;
		} state;

		// initialise the ServerGame
		void Init(
			void
		);

		// run a frame, simulate entities
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
