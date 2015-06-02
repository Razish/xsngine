#pragma once

#include "XSCommon/XSVector.h"
#include "XSCommon/XSMatrix.h"

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

namespace XS {

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

} // namespace XS
