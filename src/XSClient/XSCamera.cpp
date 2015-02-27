#include "XSCommon/XSCommon.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSMatrix.h"
#include "XSClient/XSCamera.h"
#include "XSRenderer/XSView.h"

namespace XS {

	namespace ClientGame {

		Camera::Camera()
		: Camera( matrix4() )
		{
			// default is identity transform
		}

		Camera::Camera( vector3 pos )
		: Camera()
		{
			worldTransform[ 3] = pos.x;
			worldTransform[ 7] = pos.y;
			worldTransform[11] = pos.z;
			worldTransform[15] = 1.0f;

			isPerspectiveSet = false;
		}

		Camera::Camera( matrix4 transform )
		: worldTransform( transform )
		{
			UpdateProjectionViewTransform();
		}

		void Camera::SetTransform( const matrix4 &transform ) {
			worldTransform = transform;
			UpdateProjectionViewTransform();
		}

		const matrix4 &Camera::GetTransform( void ) const {
			return worldTransform;
		}

		void Camera::SetPosition( const vector3 &position ) {
			worldTransform[ 3] = position.x;
			worldTransform[ 7] = position.y;
			worldTransform[11] = position.z;
			worldTransform[15] = 1.0f;
		}

		void Camera::LookAt( const vector3 &lookAt, const vector3 &up ) {
			vector3 eye = vector3( worldTransform[3], worldTransform[7], worldTransform[11] );

			worldTransform = matrix4::lookAt( eye, lookAt, up ).inverse();
			UpdateProjectionViewTransform();
		}

		void Camera::LookAt( const vector3 &position, const vector3 &lookAt, const vector3 &up ) {
			worldTransform = matrix4::lookAt( position, lookAt, up ).inverse();
			UpdateProjectionViewTransform();
		}

		void Camera::SetupPerspective( real32_t fov, real32_t aspectRatio, real32_t zNear, real32_t zFar ) {
			projectionTransform = perspectiveFov( fov, aspectRatio, zNear, zFar );
			isPerspectiveSet = true;

			UpdateProjectionViewTransform();
		}

		void Camera::UpdateProjectionViewTransform( void ) {
			viewTransform = worldTransform.inverse();
			projectionViewTransform = projectionTransform * viewTransform;
		}

	} // namespace ClientGame

} // namespace XS
