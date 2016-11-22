#include "Common/Common.h"
#include "Common/Vector.h"
#include "Common/Matrix.h"
#include "Client/BaseCamera.h"

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

namespace ClientGame {

	BaseCamera::BaseCamera( const Renderer::View &view, const glm::vec3 &position )
	:	view( view )
	{
		worldTransform[3] = glm::vec4( position, 1.0 );
		perspectiveSet = false;
	}

	BaseCamera::BaseCamera( const Renderer::View &view, const glm::mat4 &transform )
	:	view( view ),
		worldTransform( transform )
	{
		UpdateProjectionViewTransform();
	}

	void BaseCamera::SetTransform( const glm::mat4 &transform ) {
		worldTransform = transform;
		UpdateProjectionViewTransform();
	}

	void BaseCamera::SetPosition( const glm::vec3 &position ) {
		worldTransform[3] = glm::vec4( position, 1.0f );
		UpdateProjectionViewTransform();
	}

	void BaseCamera::LookAt( const glm::vec3 &lookAt, const glm::vec3 &worldUp ) {
		glm::vec4 vLocation = worldTransform[3];
		worldTransform = glm::inverse( glm::lookAt( vLocation.xyz(), lookAt, worldUp ) );
		UpdateProjectionViewTransform();
	}

	void BaseCamera::LookAt( const glm::vec3 &position, const glm::vec3 &lookAt, const glm::vec3 &worldUp ) {
		worldTransform = glm::inverse( glm::lookAt( position, lookAt, worldUp ) );
		UpdateProjectionViewTransform();
	}

	void BaseCamera::SetupPerspective( real32_t fov, real32_t aspectRatio, real32_t zNear, real32_t zFar ) {
		projectionTransform = glm::perspective( fov, aspectRatio, zNear, zFar );
		perspectiveSet = true;

		UpdateProjectionViewTransform();
	}

	void BaseCamera::UpdateProjectionViewTransform( void ) {
		viewTransform = glm::inverse( worldTransform );
		projectionViewTransform = projectionTransform * viewTransform;
	}

} // namespace ClientGame
