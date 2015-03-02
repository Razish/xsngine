#include "XSCommon/XSCommon.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSMatrix.h"
#include "XSClient/XSCamera2.h"
#include "XSRenderer/XSView.h"

namespace XS {

	namespace ClientGame {

		static const vector3 up( 0.0f, 0.0f, 1.0f );

		void Camera2::Set( const vector3 &newEye, const vector3 &newLook ) {
			eye = newEye;
			look = newLook;
			n = newEye - newLook;

			u = vector3::cross( up, n );
			n.normalise();
			u.normalise();

			v = vector3::cross( n, u );
			UpdateProjectionView();
		}

		void Camera2::SetupPerspective( real64_t fov, real64_t aspectRatio, real64_t zNear, real64_t zFar ) {
			projectionView = perspectiveFov( fov, aspectRatio, zNear, zFar );
		}

		void Camera2::SetEye( const vector3 &newEye ) {
			eye = newEye;
		/*
			n = eye - look;

			u = vector3::cross( up, n );
			n.normalise();
			u.normalise();
			v = vector3::cross( n, u );
		*/

			UpdateProjectionView();
		}

		// Altered to work on locked yaw axis.
		void Camera2::Slide( real32_t newU, real32_t newV, real32_t newN, bool unlocked ) {
			eye.x += newU * u.x + (newV * v.x * unlocked) + newN * n.x;
			eye.y += newU * u.y + (newV * v.y			) + newN * n.y;
			eye.z += newU * u.z + (newV * v.z * unlocked) + newN * n.z;
			UpdateProjectionView();
		}

		void Camera2::Roll( real32_t angle ) {
			vector3 t, t2, t3;

			t = u;
			angle *= M_PI / 180;

			t2 = t * cosf( angle );
			t3 = v * sinf( angle );
			u = t2 + t3;

			t2 = t * -sinf( angle );
			t3 = v *  cosf( angle );
			v = t2 + t3;

			UpdateProjectionView();
		}

		void Camera2::Pitch( real32_t angle ) {
			vector3 t, t2, t3;

			t = v;
			angle *= M_PI / 180;

			t2 = t * cosf( angle );
			t3 = n * sinf( angle );
			v = t2 + t3;

			t2 = t * -sinf( angle );
			t3 = n *  cosf( angle );
			n = t2 + t3;

			UpdateProjectionView();
		}

		void Camera2::Yaw( real32_t angle ) {
			vector3 t, t2, t3;

			t = n;
			angle *= M_PI / 180;

			t2 = t * cosf( angle );
			t3 = u * sinf( angle );
			n = t2 + t3;

			t2 = t * -sinf( angle );
			t3 = u *  cosf( angle );
			u = t2 + t3;

			UpdateProjectionView();
		}

		// new version to work with specified axis.
		// keeps the scene level when using up vector for axis.
		void Camera2::YawAxis( real32_t angle ) {
			vector3 t, t2, t3;

			t = n;
			angle *= M_PI / 180;

			t2 = t * cosf( angle );
			t3 = u * sinf( angle );
			n = t2 + t3;

			u = vector3::cross( up, n ).normalise();
			v = vector3::cross( n, u );

			UpdateProjectionView();
		}

	#if 0
		void Camera2::Rotate( int axis, real32_t angle ) {
			real32_t delN = sqrtf(
				(eye.x - look.x) * (eye.x - look.x) +
				(eye.y - look.y) * (eye.y - look.y) +
				(eye.z - look.z) * (eye.z - look.z)
			);

			Slide( 0.0f, 0.0f, -delN, true );

			if ( !axis ) {
				Yaw( -angle );
			}
			else {
				Pitch( -angle );
			}

			Slide( 0.0f, 0.0f, delN, true );
		}

		void Camera2::RotateAxis( real32_t angle, const vector3 &axis, matrix4 &m ) {
			real32_t s, c;
			real32_t ux = axis.x;
			real32_t uy = axis.y;
			real32_t uz = axis.z;

			angle *= M_PI / 180;

			s = sinf( angle );
			c = cosf( angle );

			m.at( 0, 0 ) = c + (1-c) * ux;
			m.at( 0, 1 ) = (1-c) * ux*uy + s*uz;
			m.at( 0, 2 ) = (1-c) * ux*uz - s*uy;
			m.at( 0, 3 ) = 0.0f;

			m.at( 1, 0 ) = (1-c) * uy*ux - s*uz;
			m.at( 1, 1 ) = c + (1-c) * powf( uy, 2 );
			m.at( 1, 2 ) = (1-c) * uy*uz + s*ux;
			m.at( 1, 3 ) = 0.0f;

			m.at( 2, 0 ) = (1-c) * uz*ux + s*uy;
			m.at( 2, 1 ) = (1-c) * uz*uz - s*ux;
			m.at( 2, 2 ) = c + (1-c) * powf( uz, 2 );
			m.at( 2, 3 ) = 0.0f;

			m.at( 3, 0 ) = 0.0f;
			m.at( 3, 1 ) = 0.0f;
			m.at( 3, 2 ) = 0.0f;
			m.at( 3, 3 ) = 1.0f;
		}
	#endif

		void Camera2::UpdateProjectionView( void ) {
			matrix4 &m = projectionView;

			v = vector3::cross( n, u );

			m.at( 0, 0 ) = u.x;		m.at( 1, 0 ) = u.y;		m.at( 2, 0 ) = u.z;		m.at( 3, 0 ) = -vector3::dot( eye, u );
			m.at( 0, 1 ) = v.x;		m.at( 1, 1 ) = v.y;		m.at( 2, 1 ) = v.z;		m.at( 3, 1 ) = -vector3::dot( eye, v );
			m.at( 0, 2 ) = n.x;		m.at( 1, 2 ) = n.y;		m.at( 2, 2 ) = n.z;		m.at( 3, 2 ) = -vector3::dot( eye, n );
			m.at( 0, 3 ) = 0.0f;	m.at( 1, 3 ) = 0.0f;	m.at( 2, 3 ) = 0.0f;	m.at( 3, 3 ) = 1.0f;
		}

		const matrix4 Camera2::GetProjectionView( void ) {
			UpdateProjectionView();
			matrix4 m = projectionView;
			//m.transpose();
			return m;
		}

	} // namespace ClientGame

} // namespace XS
