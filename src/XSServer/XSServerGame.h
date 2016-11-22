#pragma once

#include <list>
#include <vector>

#include "XSNetwork/XSNetwork.h"
#include "XSServer/XSEntity.h"

namespace ServerGame {

	extern struct GameState {

		std::list<Entity *>		entities;

		struct NetworkState {
			std::vector<Entity::ID>		removedEntities; // removed this frame, notify client
		} net;

	} svgState;

	// initialise the ServerGame
	void Init(
		void
	);

	// ???
	void Shutdown(
		void
	);

	// run a frame, simulate entities
	void RunFrame(
		real64_t dt
	);

	// ???
	void Connect(
		const char *ip,
		Network::GUID guid
	);

} // namespace ServerGame
