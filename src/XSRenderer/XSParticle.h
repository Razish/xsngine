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

			int32_t		life; // in milliseconds
			glm::vec3	position;
			glm::vec3	velocity;
			real64_t	size; // square
			glm::vec4	colour;

			matrix4		transform;

			// returns false if particle should be killed
			bool Update(
				real64_t dt
			);
		};

	} // namespace Renderer

} // namespace XS
