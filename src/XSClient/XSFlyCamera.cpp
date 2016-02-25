#include "XSCommon/XSCommon.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSMatrix.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSClient/XSFlyCamera.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSClientGame.h"
#include "XSClient/XSClientGameState.h"
#include "XSInput/XSInput.h"
#include "XSInput/XSKeys.h"
#include "XSRenderer/XSView.h"

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

namespace XS {

	namespace ClientGame {

		void FlyCamera::Update( real64_t dt ) {
			Cvar *r_zRange = Cvar::Get( "r_zRange" );
			real32_t zNear = r_zRange->GetReal32( 0 );
			real32_t zFar = r_zRange->GetReal32( 1 );

			SetupPerspective(
				glm::radians( cg_fov->GetReal32() ),
				Renderer::rdState.window.aspectRatio,
				zNear,
				zFar
			);

			HandleKeyboardInput( dt );
			HandleMouseInput( dt );
		}

		void FlyCamera::HandleKeyboardInput( real64_t dt ) {
			// get the camera's forward/up/right
			glm::vec3 vRight = worldTransform[0].xyz();
			glm::vec3 vUp = worldTransform[1].xyz();
			glm::vec3 vForward = worldTransform[2].xyz();

			glm::vec3 wishDir;
			if ( Client::Input::perFrameState.moveForward ) {
				wishDir -= vForward;
			}

			if ( Client::Input::perFrameState.moveBack ) {
				wishDir += vForward;
			}

			if ( Client::Input::perFrameState.moveLeft ) {
				wishDir -= vRight;
			}

			if ( Client::Input::perFrameState.moveRight ) {
				wishDir += vRight;
			}

			if ( Client::Input::perFrameState.moveUp ) {
				wishDir += vUp;
			}

			if ( Client::Input::perFrameState.moveDown ) {
				wishDir += vUp * -1.0f;
			}

			glm::normalize( wishDir );
			glm::vec3 velocity = wishDir * flySpeed * dt;

			real32_t speed = velocity.length();
			if ( speed > 0.01f ) {
				SetPosition( GetPosition() + velocity );
			}
		}

		void FlyCamera::HandleMouseInput( real64_t dt ) {
			real64_t xOffset = clgState.viewDelta[1] / 360.0;
			real64_t yOffset = clgState.viewDelta[0] / 360.0;
			CalculateRotation( dt, xOffset, yOffset );
		}

		void FlyCamera::CalculateRotation( real64_t dt, real64_t xOffset, real64_t yOffset ) {
			glm::mat4 inversed = glm::inverse( worldTransform );
			glm::mat4 rot = glm::rotate( glm::radians( (float)-(xOffset * dt) ), inversed[1].xyz() );
			worldTransform = worldTransform * rot;

			rot = glm::rotate( glm::radians( (float)-(yOffset * dt) ), glm::vec3( 1, 0, 0 ) );
			worldTransform = worldTransform * rot;

			glm::mat4 trans = worldTransform;

			worldTransform[0].xyz() = glm::normalize( glm::cross( glm::vec3( 0, 1, 0 ), trans[2].xyz() ));
			worldTransform[0].w = 0.0;

			worldTransform[1].xyz() = glm::normalize( glm::cross( trans[2].xyz(), worldTransform[0].xyz() ) );
			worldTransform[1].w = 0.0;

			worldTransform[2].w = 0.0;

			UpdateProjectionViewTransform();
		}

	} // namespace ClientGame

} // namespace XS
