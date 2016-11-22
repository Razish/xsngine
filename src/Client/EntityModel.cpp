#include "Common/Common.h"
#include "Common/Console.h"
#include "Client/EntityModel.h"

namespace ClientGame {

	EntityModel::EntityModel() {
		type = EntityType::Model;
	}

	void EntityModel::Update( real64_t dt ) {
		Entity::Update( dt );
	}

} // namespace ClientGame
