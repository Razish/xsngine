#include <queue>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSConsole.h"
#include "XSClient/XSClient.h"
#include "XSInput/XSInput.h"
#include "XSRenderer/XSRenderer.h"

//#define MOUSE_SMOOTHING

namespace XS {

	namespace Client {

		// input events will accumulate throughout the frame, and the final mouse movement (i.e. affecting viewangles)
		//	will be calculated when a movement command is generated (taking into account smoothing, acceleration,
		//	sensitivity)

		namespace Input {

			// store a small number of previous samples for interpolation (and extrapolation?)
			std::queue<pvector2> previousStates;

			void MouseWheelEvent( const struct MouseWheelEvent &ev ) {
				// ...
			}
			void MouseButtonEvent( const struct MouseButtonEvent &ev ) {
				// ...
			}
			void MouseMotionEvent( const struct MouseMotionEvent &ev ) {
				// accumulate the movement for this frame
				perFrameState.mouseDelta.x += static_cast<real64_t>( ev.x );
				perFrameState.mouseDelta.y += static_cast<real64_t>( ev.y );
				if ( debug_mouse->GetBool() ) {
					console.Print( PrintLevel::Normal,
						"MouseMotionEvent( %i, %i )\n",
						ev.x,
						ev.y
					);
				}
			}

			void CalculateMouseMotion( real64_t frameMsec, MovementCommand &cmd ) {
				pvector2 totalDelta = perFrameState.mouseDelta;

#if defined(MOUSE_SMOOTHING)
				uint32_t numFrames = 0u;
				real64_t smoothFactor = 1.0;
				const real64_t smoothDecimator = 1.5; // 1.0 / 1.5 = 0.66666
				while ( !previousStates.empty() ) {
					pvector2 &frameDelta = previousStates.front();

					// scale per smoothing frame
					numFrames++;
					frameDelta *= 1.0 / (numFrames * smoothFactor);
					smoothFactor /= smoothDecimator;

					if ( m_acceleration->GetBool() ) {
						//TODO: handle mouse acceleration (linear, logarithmic, quadratic/ql style)
					}

					// scale by sensitivity factor
					frameDelta *= m_sensitivity->GetReal64();

					//TODO: scale by aspect ratio?
					/*
					const Cvar *r_fov = Cvar::Get( "cg_fov" );
					const real64_t fov = cg_fov ? cg_fov->GetReal64() : 75.0;
					frameDelta *= fov / Renderer::state.window.aspectRatio;
					*/

					totalDelta += frameDelta;
				}
				while ( previousStates.size() >= m_smoothFrames->GetUInt32() ) {
					previousStates.pop();
				}
				previousStates.push( perFrameState.mouseDelta );
#endif
				perFrameState.mouseDelta.clear();

				const vector3 oldAngles = Client::state.viewAngles;

#if defined(MOUSE_SMOOTHING)
				if ( !dblcmp( totalDelta.x, 0.0 ) ) {
					totalDelta.x /= static_cast<real64_t>( numFrames );
				}
				if ( !dblcmp( totalDelta.y, 0.0 ) ) {
					totalDelta.y /= static_cast<real64_t>( numFrames );
				}
#endif

				if ( debug_mouse->GetBool() && (!dblcmp( totalDelta.x, 0.0 ) || !dblcmp( totalDelta.y, 0.0 )) ) {
					console.Print( PrintLevel::Normal,
						"CalculateMouseMotion( %.3f, %.3f )\n",
						static_cast<real32_t>( totalDelta.x ),
						static_cast<real32_t>( totalDelta.y )
					);
				}

				//TODO: convert/wrap to euler rotation?
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

		} // namespace Input

	} // namespace Client

} // namespace XS
