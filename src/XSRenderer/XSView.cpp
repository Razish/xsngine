#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSBackend.h"

namespace XS {

	namespace Renderer {

		View::View( uint32_t width, uint32_t height, bool is2D ) : width(width), height(height), is2D(is2D) {
			if ( !width || !height ) {
				throw( XSError( "Registered View with 0 width or 0 height" ) );
			}

			perFrameData = new Buffer( BufferType::Uniform, nullptr, sizeof(float) * 16 * 2 );

			RegisterView( this );
		}

		View::~View() {
			delete perFrameData;
		}

		void View::PreRender( void ) {
			// set up 2d/3d perspective
			if ( is2D ) {
				projectionMatrix = ortho( 0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height),
					0.0f, 1.0f);
			}
			else {
				const float fov = Backend::r_fov->GetFloat();
				const float zNear = Backend::r_zRange->GetFloat( 0 );
				const float zFar = Backend::r_zRange->GetFloat( 1 );
				projectionMatrix = perspectiveFov( fov, static_cast<float>(width) / static_cast<float>(height),
					zNear, zFar );
			}

			matrix4 *m = static_cast<matrix4 *>(perFrameData->Map());

			*m = projectionMatrix;

			perFrameData->Unmap();
			perFrameData->BindRange( 6 );
		}

		void View::PostRender( void ) const {
			// ...
		}

		void View::Bind( void ) {
			SetView( this );
		}

	} // namespace Renderer

} // namespace XS
