#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSError.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBuffer.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSBackend.h"
#include "XSRenderer/XSRenderable.h"
#include "XSRenderer/XSShaderProgram.h"

namespace XS {

	namespace Renderer {

		View::View( uint32_t viewWidth, uint32_t viewHeight, bool is2D, renderCallback_t preRender,
			renderCallback_t postRender )
		: width( viewWidth ), height( viewHeight ), callbackPreRender( preRender ), callbackPostRender( postRender ), is2D( is2D )
		{
			if ( !viewWidth || !viewHeight ) {
				width = state.window.width;
				height = state.window.height;
			}

			perFrameData = new Backend::Buffer( BufferType::Uniform, nullptr, sizeof(real32_t) * 16 );

			RegisterView( this );
		}

		void View::PreRender( real64_t dt ) {
			// set up 2d/3d perspective
			if ( is2D ) {
				matrix4 o = ortho(
					0.0f,
					static_cast<real32_t>( width ),
					0.0f,
					static_cast<real32_t>( height ),
					0.0f, // zNear
					1.0f // zFar
				);
				projectionMatrix = glm::make_mat4( o.data );
			}

			if ( callbackPreRender ) {
				callbackPreRender( dt );
			}

			matrix4 *m = static_cast<matrix4 *>( perFrameData->Map() );

			memcpy( m->data, glm::value_ptr( projectionMatrix ), sizeof(m->data) );

			perFrameData->Unmap();
			perFrameData->BindRange( 6 );

			// sort surfaces etc?

			for ( const auto &object : renderObjects ) {
				if ( object ) {
					object->Draw();
				}
			}
		}

		void View::PostRender( real64_t dt ) const {
			if ( callbackPostRender ) {
				callbackPostRender( dt );
			}
		}

		void View::Bind( void ) {
			BindView( this );
		}

		void View::AddObject( const Renderable *renderObject ) {
			renderObjects.push_back( renderObject );
		}

	} // namespace Renderer

} // namespace XS
