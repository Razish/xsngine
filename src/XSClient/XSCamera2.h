#pragma once

namespace XS {

	namespace ClientGame {

		class Camera2 {

		private:
			vector3		eye, look;
			vector3		u, v, n;
			matrix4		projectionView;

			void		Rotate( int axis, real32_t angle );
			void		RotateAxis( real32_t angle, const vector3 &axis, matrix4 &m );

public:
			inline Camera2() {
				projectionView.identity();
			}

			void			 Set( const vector3 &newEye, const vector3 &newLook ); //TODO: up
			void			 SetEye( const vector3 &eye );
			void			 SetupPerspective( real64_t viewAngle, real64_t aspect, real64_t zNear, real64_t zFar );
			void			 Slide( real32_t u, real32_t v, real32_t n, bool unlocked );
			void			 Roll( real32_t angle );
			void			 Pitch( real32_t angle );
			void			 Yaw( real32_t angle );
			void			 YawAxis( real32_t angle );
			void			 UpdateProjectionView( void );
			const matrix4 	 GetProjectionView( void );
		};

	} // namespace ClientGame

} // namespace XS
