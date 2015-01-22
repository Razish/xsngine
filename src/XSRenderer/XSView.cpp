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

		View::View( uint32_t width, uint32_t height, bool is2D, renderCallback_t preRender,
			renderCallback_t postRender )
		: width( width ), height( height ), callbackPreRender( preRender ), callbackPostRender( postRender ), is2D( is2D )
		{
			if ( !width || !height ) {
				throw( XSError( "Registered View with 0 width or 0 height" ) );
			}

			perFrameData = new Backend::Buffer( Backend::Buffer::Type::UNIFORM, nullptr, sizeof(float) * 16 * 2 );

			RegisterView( this );
		}

		void View::PreRender( void ) {
			// set up 2d/3d perspective
			if ( is2D ) {
				projectionMatrix = ortho( 0.0f, static_cast<float>( width ), 0.0f, static_cast<float>( height ),
					0.0f, 1.0f );
			}
			else {
				const float fov = Backend::r_fov->GetFloat();
				const float zNear = Backend::r_zRange->GetFloat( 0 );
				const float zFar = Backend::r_zRange->GetFloat( 1 );
				projectionMatrix = perspectiveFov( fov, static_cast<float>( width ) / static_cast<float>( height ),
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
