#include <queue>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSCvar.h"
#include "XSClient/XSClient.h"
#include "XSInput/XSInput.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Client {

		// input events will accumulate throughout the frame, and the final mouse movement (i.e. affecting viewangles)
		//	will be calculated when a movement command is generated (taking into account smoothing, acceleration,
		//	sensitivity)

		// per frame, accumulates until a movement command is generated
		static struct PerFrameState {
			std::queue<pvector2> mouseDelta;
		} perFrameState = {};

		void MouseWheelEvent( const struct MouseWheelEvent &ev ) {
			// ...
		}
		void MouseButtonEvent( const struct MouseButtonEvent &ev ) {
			// ...
		}
		void MouseMotionEvent( const struct MouseMotionEvent &ev ) {
			if ( perFrameState.mouseDelta.size() >= m_smoothFrames->GetUInt32() ) {
				perFrameState.mouseDelta.pop();
			}
			perFrameState.mouseDelta.push( pvector2( ev.x, ev.y ) );
		}

		void CalculateMouseMotion( real64_t frameMsec, MovementCommand &cmd ) {
			pvector2 totalDelta;
			real64_t smoothFactor = 1.0;

			uint32_t numFrames = 0u;
			while ( !perFrameState.mouseDelta.empty() ) {
				pvector2 &frameDelta = perFrameState.mouseDelta.front();

				// scale per smoothing frame
				frameDelta *= 1.0 / (++numFrames * smoothFactor);
				smoothFactor /= 1.5;

				if ( m_acceleration->GetBool() ) {
					//TODO: handle mouse acceleration
				}
				else {
					// ???
				}

				// scale by sensitivity factor
				frameDelta *= m_sensitivity->GetReal64();

				// scale by aspect ratio
				const Cvar *r_fov = Cvar::Get( "r_fov" );
				const real64_t fov = r_fov ? r_fov->GetReal64() : 75.0;
				frameDelta *= fov / Renderer::state.window.aspectRatio;

				totalDelta += frameDelta;

				perFrameState.mouseDelta.pop();
			}

			const vector3 oldAngles = Client::state.viewAngles;
			totalDelta /= static_cast<real64_t>( numFrames );
			Client::state.viewAngles.yaw = totalDelta.x;
			Client::state.viewAngles.pitch = totalDelta.y;

			// check to make sure the angles haven't wrapped
			if ( Client::state.viewAngles.pitch - oldAngles.pitch > 90.0f ) {
				Client::state.viewAngles.pitch = oldAngles.pitch + 90.0f;
			}
			else if ( oldAngles.pitch - Client::state.viewAngles.pitch > 90.0f ) {
				Client::state.viewAngles.pitch = oldAngles.pitch - 90.0f;
			}
		}

	} // namespace Client

} // namespace XS
