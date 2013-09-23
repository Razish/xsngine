#include "XSSystem/XSInclude.h"

#include "GLee/GLee.h"
#include "SDL2/SDL.h"

#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSFramebuffer.h"

namespace XS {

	namespace Renderer {

		static std::vector<Framebuffer*> framebuffers;
		static const Framebuffer *currentReadFramebuffer;
		static const Framebuffer *currentWriteFramebuffer;
		static bool initialised;

		void Framebuffer::Init( void ) {
			if ( !SDL_GL_ExtensionSupported( "GL_EXT_framebuffer_object" ) ||
				 !SDL_GL_ExtensionSupported( "GL_EXT_framebuffer_blit" ) )
			{
				Print( "Framebuffer extension NOT loaded.\n"
						"Required OpenGL extensions not available.\n" );
				return;
			}

			Print( "Framebuffer extension loaded\n" );

			CheckGLErrors( __FILE__, __LINE__ );

			initialised = true;
			return;
		}

		void Framebuffer::Cleanup( void ) {
			Print( "Cleaning up framebuffers\n" );

			for ( auto it = framebuffers.begin(); it != framebuffers.end(); ++it ) {
				if ( !(*it)->id )
					continue;

				glDeleteFramebuffersEXT( 1, &(*it)->id );

				delete *it;
				CheckGLErrors( __FILE__, __LINE__ );
			}
			framebuffers.clear();

			currentReadFramebuffer = currentWriteFramebuffer = NULL;

			initialised = false;
		}

		void Framebuffer::BindDefault( void ) {
			if ( currentReadFramebuffer || currentWriteFramebuffer ) {
				glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
				currentReadFramebuffer	= NULL;
				currentWriteFramebuffer	= NULL;
			}

			CheckGLErrors( __FILE__, __LINE__ );
		}

		const Framebuffer *Framebuffer::GetCurrent( void ) {
			return currentReadFramebuffer;
		}

		void Framebuffer::Blit( const Framebuffer *source, const Framebuffer *destination, int sourceWidth, int sourceHeight, int destWidth, int destHeight, unsigned int bufferBits ) {
			if ( currentReadFramebuffer != source ) {
				glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, source ? source->id : 0 );
				currentReadFramebuffer = source;
			}

			if ( currentWriteFramebuffer != destination ) {
				glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, destination ? destination->id : 0 );
				currentWriteFramebuffer = destination;
			}

			glBlitFramebufferEXT( sourceWidth, sourceHeight, 0, 0, destWidth, destHeight, 0, 0, bufferBits, GL_NEAREST );

			CheckGLErrors( __FILE__, __LINE__ );
		}

		void Framebuffer::BlitColor( const Framebuffer *source, const Framebuffer *destination, int sourceWidth, int sourceHeight, int destWidth, int destHeight ) {
			Blit( source, destination, sourceWidth, sourceHeight, destWidth, destHeight, GL_COLOR_BUFFER_BIT);
		}

		void Framebuffer::BlitColorAndDepth( const Framebuffer *source, const Framebuffer *destination, int sourceWidth, int sourceHeight, int destWidth, int destHeight ) {
			Blit( source, destination, sourceWidth, sourceHeight, destWidth, destHeight, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		}

		// instance functions
		Framebuffer::Framebuffer() {
			if ( !initialised )
				throw( "CreateFramebuffer: Framebuffer extension not initialised" );

			glGenFramebuffersEXT( 1, &id );

			if ( !id )
				throw( String::Format( "CreateFramebuffer: Failed to create framebuffer with internal ID %d", framebuffers.size() ) );

			framebuffers.push_back( this );
			CheckGLErrors( __FILE__, __LINE__ );
		}

		void Framebuffer::AttachColorTexture( const Texture *texture, unsigned int slot ) {
			if ( slot >= MAX_FBO_COLOR_TEXTURES ) {
				Print( String::Format( "Invalid slot number given (%d), valid range is 0 - %d", slot, MAX_FBO_COLOR_TEXTURES-1 ) );
				return;
			}

			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + slot, GL_TEXTURE_2D, texture->id, 0 );
			colorTextures[slot] = texture;

			CheckGLErrors( __FILE__, __LINE__ );
		}

		void Framebuffer::AttachDepthTexture( const Texture *texture ) {
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, texture->id, 0 );
			depthTexture = texture;

			CheckGLErrors( __FILE__, __LINE__ );
		}

		void Framebuffer::AttachDepthStencilTexture( const Texture *texture ) {
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->id, 0 );
			depthTexture = texture;
			stencilTexture = texture->id;

			CheckGLErrors( __FILE__, __LINE__ );
		}

		void Framebuffer::Bind( void ) {
			if ( currentReadFramebuffer != this || currentWriteFramebuffer != this ) {
				glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, id );
				currentReadFramebuffer	= this;
				currentWriteFramebuffer	= this;
			}

			CheckGLErrors( __FILE__, __LINE__ );
		}

		void Framebuffer::Check( void ) {
			unsigned int status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );

			switch ( status ) {
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				Print( "One or more framebuffer attachment points are not complete.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				Print( "One or more attached images have different dimensions.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				Print( "Invalid framebuffer attachment object type used.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				Print( "More than one internal format was used in the color attachments.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				Print( "Missing a read buffer.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				Print( "No images were attached to the framebuffer.\n" );
				break;

			case GL_FRAMEBUFFER_COMPLETE_EXT:
				break;
			}

			if ( status != GL_FRAMEBUFFER_COMPLETE_EXT )
				Print( String::Format( "Creation of framebuffer %d could not be completed.\n", id ) );

			CheckGLErrors( __FILE__, __LINE__ );
		}

	}

}
