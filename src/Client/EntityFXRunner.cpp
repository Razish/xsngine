#include "Common/Common.h"
#include "Common/Console.h"
#include "Client/EntityFXRunner.h"
#include "Client/ParticleEmitter.h"

namespace ClientGame {

	EntityFXRunner::EntityFXRunner() {
		type = EntityType::FXRunner;
	}

	void EntityFXRunner::Update( real64_t dt ) {
		Entity::Update( dt );
	}

} // namespace ClientGame
