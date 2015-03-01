#include "XSCommon/XSCommon.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSMatrix.h"
#include "XSClient/XSCamera.h"
#include "XSRenderer/XSView.h"

namespace XS {

	namespace ClientGame {

		bool Camera::doTranspose = false;

		Camera::Camera()
		: Camera( matrix4() )
		{
			// default is identity transform
		}

		Camera::Camera( vector3 pos )
		: Camera()
		{
			worldTransform.at( 0, 3, doTranspose ) = pos.x;
			worldTransform.at( 1, 3, doTranspose ) = pos.y;
			worldTransform.at( 2, 3, doTranspose ) = pos.z;
			worldTransform.at( 3, 3, doTranspose ) = 1.0f;

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
			worldTransform.at( 0, 3, doTranspose ) = position.x;
			worldTransform.at( 1, 3, doTranspose ) = position.y;
			worldTransform.at( 2, 3, doTranspose ) = position.z;
			worldTransform.at( 3, 3, doTranspose ) = 1.0f;
		}

		void Camera::LookAt( const vector3 &lookAt, const vector3 &up ) {
			vector3 eye = vector3(
				worldTransform.at( 0, 3, doTranspose ),
				worldTransform.at( 1, 3, doTranspose ),
				worldTransform.at( 2, 3, doTranspose )
			);

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
