#pragma once

#include <vector>

#include "XSCommon/XSVector.h"

namespace XS {

	namespace ClientGame {

		class Entity;

		extern struct GameState {
			vector3					viewAngles;
			vector3					viewDelta;
			std::vector<Entity *>	entities;	// always allocate with `new`
		} state;

	} // namespace ClientGame

} // namespace XS
