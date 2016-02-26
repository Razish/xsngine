#pragma once

#include <unordered_map>

#include "XSCommon/XSVector.h"

namespace XS {

	namespace ClientGame {

		class Entity;
		using EntityContainer = std::unordered_map<uint32_t, Entity *>;

		extern struct GameState {
			vector3			viewAngles;
			vector3			viewDelta;
			EntityContainer	entities;
		} clgState;

	} // namespace ClientGame

} // namespace XS
