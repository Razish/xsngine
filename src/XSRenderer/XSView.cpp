#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSError.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBuffer.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSBackend.h"
#include "XSRenderer/XSRenderable.h"

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

			perFrameData = new Backend::Buffer( BufferType::Uniform, nullptr, sizeof(real32_t) * 16 * 2 );

			RegisterView( this );
		}

		void View::PreRender( void ) {
			// set up 2d/3d perspective
			if ( is2D ) {
				projectionMatrix = ortho( 0.0f, static_cast<real32_t>( width ), 0.0f, static_cast<real32_t>( height ),
					0.0f, 1.0f );
			}
			else {
				const real32_t fov = Backend::r_fov->GetReal32();
				const real32_t zNear = Backend::r_zRange->GetReal32( 0 );
				const real32_t zFar = Backend::r_zRange->GetReal32( 1 );
				projectionMatrix = perspectiveFov( fov,
					static_cast<real32_t>( width ) / static_cast<real32_t>( height ),
					zNear, zFar );
			}

			matrix4 *m = static_cast<matrix4 *>( perFrameData->Map() );

			*m = projectionMatrix;

			perFrameData->Unmap();
			perFrameData->BindRange( 6 );

			if ( callbackPreRender ) {
				callbackPreRender();
			}

			// sort surfaces etc?

			for ( const auto &object : renderObjects ) {
				if ( object ) {
					object->Draw();
				}
			}
		}

		void View::PostRender( void ) const {
			if ( callbackPostRender ) {
				callbackPostRender();
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
