#pragma once

#include <unordered_map>

#include "Common/Vector.h"
#include "Network/Network.h"

namespace ClientGame {

	class Entity;
	using EntityContainer = std::unordered_map<uint32_t, Entity *>;

	extern struct GameState {
		Network::Connection	*connection;
		vector3				 viewAngles;
		vector3				 viewDelta;
		EntityContainer		 entities;
	} clgState;

} // namespace ClientGame
