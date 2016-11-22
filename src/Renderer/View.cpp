#include "Common/Common.h"
#include "Common/Cvar.h"
#include "Common/Error.h"
#include "Renderer/Renderer.h"
#include "Renderer/Buffer.h"
#include "Renderer/View.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Backend.h"
#include "Renderer/Renderable.h"
#include "Renderer/ShaderProgram.h"
#include "Renderer/Renderer.h"

namespace Renderer {

	View::View( uint32_t viewWidth, uint32_t viewHeight, bool is2D, RenderCallback preRender,
		RenderCallback postRender )
	: callbackPreRender( preRender ), callbackPostRender( postRender ), is2D( is2D ), width( viewWidth ),
		height( viewHeight )
	{
		if ( !viewWidth || !viewHeight ) {
			width = rdState.window.width;
			height = rdState.window.height;
		}

		perFrameData = new Backend::Buffer( BufferType::Uniform, nullptr, 4 * 1024 * 1024 );

		fbo = new Framebuffer();
		fbo->Bind();

		colourTexture = new Texture( width, height );
		fbo->AttachColourTexture( colourTexture, 0 );

		if ( is2D ) {
			depthTexture = new Texture( width, height, InternalFormat::Depth24Stencil8 );
			fbo->AttachDepthStencilTexture( depthTexture );
		}
		else {
			depthTexture = new Texture( width, height, InternalFormat::Depth24Stencil8 );
			fbo->AttachDepthStencilTexture( depthTexture );
		}

		RegisterView( this );
	}

	View::~View() {
		delete depthTexture;
		delete colourTexture;
		delete fbo;
		delete perFrameData;
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

		BufferMemory bufferMem;
		perFrameData->MapDiscard( &bufferMem, sizeof( matrix4 ) );
		matrix4 *m = static_cast<matrix4 *>( bufferMem.devicePtr );

		memcpy( m->data, glm::value_ptr( projectionMatrix ), sizeof(m->data) );

		perFrameData->Unmap();
		perFrameData->BindRange( 6, bufferMem.offset, bufferMem.size );

		// sort surfaces etc?

		while ( !renderObjects.empty() ) {
			const auto &object = renderObjects.front();

			if ( object.handle != Renderable::invalidHandle ) {
				//FIXME: this design seems ass-backwards WRT RenderInfo.handle -> Renderable::Draw( RenderInfo )
				Renderable::Get( object.handle )->Draw( object );
			}

			renderObjects.pop();
		}
	}

	void View::PostRender( real64_t dt ) {
		if ( callbackPostRender ) {
			callbackPostRender( dt );
		}
	}

	void View::Bind( void ) {
		SetCurrentView( this );
		if ( fbo ) {
			fbo->Bind();
			fbo->Check();
		}
		else {
			Framebuffer::BindDefault();
		}
	}

	void View::AddObject( const RenderInfo &renderInfo ) {
		renderObjects.push( renderInfo );
	}

	void View::AddPointLight( const vector3 &lightPos ) {
		pointLights.push( lightPos );
	}

} // namespace Renderer
