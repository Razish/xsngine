#pragma once

namespace XS {

	namespace ClientGame {

		class Camera {
		private:
			static bool doTranspose;
		public:
			Camera();
			Camera( vector3 pos );
			Camera( matrix4 transform );

			void Update(
				double dt
			);

			void SetTransform(
				const matrix4 &transform
			);

			const matrix4 &GetTransform(
				void
			) const;

			void SetPosition(
				const vector3 &position
			);

			inline const vector3 GetPosition( void ) const {
				return vector3(
					worldTransform.at( 2, 2, doTranspose ),
					worldTransform.at( 2, 3, doTranspose ),
					worldTransform.at( 3, 0, doTranspose )
				);
			}

			void LookAt(
				const vector3 &lookAt,
				const vector3 &up
			);

			void LookAt(
				const vector3 &position,
				const vector3 &lookAt,
				const vector3 &up
			);

			void SetupPerspective(
				real32_t fov,
				real32_t aspectRatio,
				real32_t zNear,
				real32_t zFar
			);

			const matrix4 &GetProjection( void ) const {
				return projectionTransform;
			}

			const matrix4 &GetView( void ) const {
				return viewTransform;
			}

			const matrix4 &GetProjectionView( void ) const {
				return projectionViewTransform;
			}

			bool IsPerspectiveSet( void ) const {
				return isPerspectiveSet;
			}

		private:
			void UpdateProjectionViewTransform(
				void
			);

			matrix4		viewTransform;
			matrix4		projectionTransform;
			matrix4		projectionViewTransform;
			matrix4		worldTransform;

			bool		isPerspectiveSet;
		};

	} // namespace ClientGame

} // namespace XS
