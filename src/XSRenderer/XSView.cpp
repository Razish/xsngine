#include "XSSystem/XSInclude.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSError.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSBackend.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSMaterial.h"

namespace XS {

	namespace Renderer {

		View::View() {
			perFrameData = new Buffer( BufferType::Uniform, nullptr, sizeof(float) * 16 * 2 );
		}

		View::~View() {
			delete perFrameData;
		}

		void View::PreRender( void ) {
			// set up 2d/3d perspective
			if ( is2D ) {
				projectionMatrix = ortho( 0.0f, 1280.0f, 0.0f, 720.0f, 0.0f, 1.0f );
			}
			else {
				projectionMatrix = perspectiveFov( 60.0f, static_cast<float>(width) / static_cast<float>(height), 4.0f, 1000.0f );
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

		void View::Register( void ) {
			if ( !width || !height ) {
				throw( XSError( "Registered view with 0 width or 0 height" ) );
			}

			RegisterView( this );
		}

	} // namespace Renderer

} // namespace XS
