#pragma once

#include "Client/Entity.h"

namespace ClientGame {

	class ParticleEmitter;

	class EntityFXRunner : public Entity {
	private:

	public:
		uint32_t count;
		uint32_t life;

		EntityFXRunner();

		void Update(
			real64_t dt
		);

	};

} // namespace ClientGame
