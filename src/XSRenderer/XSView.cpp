#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSError.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBuffer.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSFramebuffer.h"
#include "XSRenderer/XSBackend.h"
#include "XSRenderer/XSRenderable.h"
#include "XSRenderer/XSShaderProgram.h"

namespace XS {

	namespace Renderer {

		View::View( uint32_t viewWidth, uint32_t viewHeight, bool is2D, RenderCallback preRender,
			RenderCallback postRender )
		: callbackPreRender( preRender ), callbackPostRender( postRender ), is2D( is2D ), width( viewWidth ),
			height( viewHeight )
		{
			if ( !viewWidth || !viewHeight ) {
				width = state.window.width;
				height = state.window.height;
			}

			perFrameData = new Backend::Buffer( BufferType::Uniform, nullptr, 4 * 1024 * 1024 );

			fbo = new Framebuffer();
			fbo->Bind();

			// diffuse
			textures[RenderTarget::Diffuse] = new Texture( width, height, InternalFormat::RGBA8 );
			fbo->AttachColourTexture( textures[RenderTarget::Diffuse], RenderTarget::Diffuse );

			// normal
			textures[RenderTarget::Normal] = new Texture( width, height, InternalFormat::RGB8 );
			fbo->AttachColourTexture( textures[RenderTarget::Normal], RenderTarget::Normal );

			// position
			textures[RenderTarget::Position] = new Texture( width, height, InternalFormat::RGBA8 );
			fbo->AttachColourTexture( textures[RenderTarget::Position], RenderTarget::Position );

			// tex coord
			textures[RenderTarget::TexCoord] = new Texture( width, height, InternalFormat::RGBA8 );
			fbo->AttachColourTexture( textures[RenderTarget::TexCoord], RenderTarget::TexCoord );

			// lighting pass
			textures[RenderTarget::Lighting] = new Texture( width, height, InternalFormat::RGBA8 );
			fbo->AttachColourTexture( textures[RenderTarget::Lighting], RenderTarget::Lighting );

			// depth
			textures[RenderTarget::Depth] = new Texture( width, height, InternalFormat::Depth24Stencil8 );
			fbo->AttachDepthStencilTexture( textures[RenderTarget::Depth] );

			RegisterView( this );
		}

		View::~View() {
			for ( auto texture : textures ) {
				delete texture;
			}
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

			XS::Renderer::BufferMemory bufferMem = perFrameData->MapDiscard( sizeof( matrix4 ) * 2 );
			matrix4 *m = static_cast<matrix4 *>( bufferMem.devicePtr );

			memcpy( m->data, glm::value_ptr( projectionMatrix ), sizeof(m->data) );
			m++;

			if ( !is2D ) {
				memcpy( m->data, glm::value_ptr( worldMatrix ), sizeof(m->data) );
				m++;
			}

			perFrameData->Unmap();
			perFrameData->BindRange( 6, bufferMem.offset, bufferMem.size );

			// sort surfaces etc?

			while ( !renderObjects.empty() ) {
				const auto &object = renderObjects.front();

				object->Draw();

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

		void View::AddObject( const Renderable *renderObject ) {
			renderObjects.push( renderObject );
		}

		void View::AddPointLight( const vector3 &lightPos ) {
			//TODO: add point light to scene
		}

	} // namespace Renderer

} // namespace XS
