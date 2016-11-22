#pragma once

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

#include "XSRenderer/XSView.h"

namespace ClientGame {

	class BaseCamera {

	public:
		// don't allow default instantiation
		BaseCamera() = delete;
		BaseCamera( const BaseCamera& ) = delete;
		BaseCamera& operator=( const BaseCamera& ) = delete;

		BaseCamera(
			const Renderer::View &view,
			const glm::vec3 &position
		);

		BaseCamera(
			const Renderer::View &view,
			const glm::mat4 &transform
		);

		virtual void Update(
			real64_t dt
		) = 0;

		const glm::mat4 &GetTransform( void ) const {
			return worldTransform;
		};

		void SetTransform(
			const glm::mat4 &transform
		);

		glm::vec3 GetPosition( void ) const {
			return worldTransform[3].xyz();
		}

		void SetPosition(
			const glm::vec3 &position
		);

		void LookAt(
			const glm::vec3 &lookAt,
			const glm::vec3 &up
		);

		void LookAt(
			const glm::vec3 &position,
			const glm::vec3 &lookAt,
			const glm::vec3 &up
		);

		void SetupPerspective(
			real32_t fieldOfView,
			real32_t aspectRatio,
			real32_t near = 0.1f,
			real32_t far = 4000.0f
		);

		const glm::mat4 &GetProjection( void ) const {
			return projectionTransform;
		}

		const glm::mat4 &GetView( void ) const {
			return viewTransform;
		}

		const glm::mat4 &GetProjectionView( void ) const {
			return projectionViewTransform;
		}

		bool GetPerspectiveSet() const {
			return perspectiveSet;
		}

	protected:
		void UpdateProjectionViewTransform(
			void
		);

		const Renderer::View	&view;
		glm::mat4				 viewTransform;
		glm::mat4				 projectionTransform;
		glm::mat4				 projectionViewTransform;
		glm::mat4				 worldTransform;

		bool					 perspectiveSet; //FIXME: hide?

	};

} // namespace ClientGame
