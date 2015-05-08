#include "XSCommon/XSCommon.h"
#include "XSRenderer/XSParticle.h"

namespace XS {

	namespace Renderer {

		Particle::Particle()
		: life( 0 ), position( 0.0f, 0.0f, 0.0f ), velocity( 0.0f, 0.0f, 0.0f ), size( 1.0f ),
			colour( 1.0f, 1.0f, 1.0f, 1.0f )
		{
			real32_t speed = 8.0f + (2.0f * (rand() / static_cast<real32_t>( RAND_MAX )));

			real32_t r;
			r = ((rand() / static_cast<real32_t>( RAND_MAX )) * 2.0) - 1.0f;
			velocity.x = r;

			//r = ((rand() / static_cast<real32_t>( RAND_MAX )) * 2.0) - 1.0f;
			velocity.y = 2.0f;

			r = ((rand() / static_cast<real32_t>( RAND_MAX )) * 2.0) - 1.0f;
			velocity.z = r;

			velocity *= speed;
		}

		bool Particle::Update( real64_t dt ) {
			life -= std::ceil( dt );
			if ( life < 0 ) {
				return false;
			}

			const real32_t gravity = -24.0f;
			position += velocity * (dt / 1000.0);
			velocity.y += gravity * (dt / 1000.0);

			return true;
		}

	} // namespace Renderer

} // namespace XS
