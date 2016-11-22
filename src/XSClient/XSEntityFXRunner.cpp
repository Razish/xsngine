#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSClient/XSEntityFXRunner.h"
#include "XSClient/XSParticleEmitter.h"

namespace ClientGame {

	EntityFXRunner::EntityFXRunner() {
		type = EntityType::FXRunner;
	}

	void EntityFXRunner::Update( real64_t dt ) {
		Entity::Update( dt );
	}

} // namespace ClientGame
