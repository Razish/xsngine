#pragma once

#include "XSClient/XSBaseCamera.h"

namespace XS {

	namespace ClientGame {

		class FlyCamera : public BaseCamera {
		public:
			FlyCamera()
			: FlyCamera( 1.0f )
			{
				//viewButtonClicked = false;
			}
			FlyCamera( real32_t flySpeed )
			: BaseCamera(), flySpeed( flySpeed )
			{
			}
			~FlyCamera() {
			};

			virtual void Update( real64_t dt );

			void SetFlySpeed( real32_t speed );
			inline real32_t GetFlySpeed( void ) const {
				return flySpeed;
			}

		protected:
			void HandleKeyboardInput( real64_t dt );
			void HandleMouseInput( real64_t dt );

			void CalculateRotation( real64_t dt, real64_t xOffset, real64_t yOffset);

			real32_t flySpeed;
		};

	} // namespace ClientGame

} // namespace XS
