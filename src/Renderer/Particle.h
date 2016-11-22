#pragma once

#include "Common/Vector.h"
#include "Common/Matrix.h"

#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
	#pragma GCC diagnostic pop
#endif

namespace Renderer {

	class Particle {

	public:
		Particle();

		int32_t		life = 0; // in milliseconds
		glm::vec3	position{ 0.0f, 0.0f, 0.0f };
		glm::vec3	velocity{ 0.0f, 1.0f, 0.0f };
		real64_t	size = 1.0f; // square
		glm::vec4	colour{ 1.0f, 1.0f, 1.0f, 1.0f };

		matrix4		transform;

		// returns false if particle should be killed
		bool Update(
			real64_t dt
		);

	};

} // namespace Renderer
