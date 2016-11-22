#pragma once

#include <unordered_map>

#include "XSCommon/XSVector.h"
#include "XSNetwork/XSNetwork.h"

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
