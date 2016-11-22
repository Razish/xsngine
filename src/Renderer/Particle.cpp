#include "Common/Common.h"
#include "Renderer/Particle.h"

namespace Renderer {

	Particle::Particle() {
		real32_t speed = 4.0f;//rand() / static_cast<real32_t>( RAND_MAX );

		auto rd = []() { return 1.0f - ((rand() / static_cast<real32_t>( RAND_MAX )) * 2.0f); };
		velocity.x = rd();
		velocity.y = 2.0f;
		velocity.z = rd();

		velocity *= speed;
	}

	bool Particle::Update( real64_t dt ) {
		const real64_t t = dt / 1000.0;
		life -= std::ceil( dt );
		if ( life < 0 ) {
			return false;
		}

		const real32_t gravity = 9.8067f * 2.0f;

		glm::vec3 oldVelocity = velocity;
		velocity.y -= gravity * t;
		position += (oldVelocity + velocity) * 0.5f * t;

		return true;
	}

} // namespace Renderer
