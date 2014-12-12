#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSError.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSFramebuffer.h"

namespace XS {

	namespace Renderer {

		const Framebuffer *Framebuffer::currentReadFramebuffer = nullptr;
		const Framebuffer *Framebuffer::currentWriteFramebuffer = nullptr;

		void Framebuffer::Init( void ) {
			// no initialization required
		}

		void Framebuffer::BindDefault( void ) {
			if ( currentReadFramebuffer || currentWriteFramebuffer ) {
				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
				currentReadFramebuffer	= nullptr;
				currentWriteFramebuffer	= nullptr;
			}
		}

		const Framebuffer *Framebuffer::GetCurrent( void ) {
			return currentReadFramebuffer;
		}

		void Framebuffer::Blit( const Framebuffer *source, const Framebuffer *destination, int sourceWidth,
			int sourceHeight, int destWidth, int destHeight, unsigned int bufferBits )
		{
			if ( currentReadFramebuffer != source ) {
				glBindFramebuffer( GL_READ_FRAMEBUFFER, source ? source->id : 0 );
				currentReadFramebuffer = source;
			}

			if ( currentWriteFramebuffer != destination ) {
				glBindFramebuffer( GL_DRAW_FRAMEBUFFER, destination ? destination->id : 0 );
				currentWriteFramebuffer = destination;
			}

			glBlitFramebuffer( sourceWidth, sourceHeight, 0, 0, destWidth, destHeight, 0, 0, bufferBits, GL_NEAREST );
		}

		void Framebuffer::BlitColor( const Framebuffer *source, const Framebuffer *destination, int sourceWidth,
			int sourceHeight, int destWidth, int destHeight )
		{
			Blit( source, destination, sourceWidth, sourceHeight, destWidth, destHeight, GL_COLOR_BUFFER_BIT);
		}

		void Framebuffer::BlitColorAndDepth( const Framebuffer *source, const Framebuffer *destination, int sourceWidth,
			int sourceHeight, int destWidth, int destHeight )
		{
			Blit( source, destination, sourceWidth, sourceHeight, destWidth, destHeight,
				GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		}

		// instance functions
		Framebuffer::Framebuffer()
		: id( 0 )
		{
			glGenFramebuffers( 1, &id );

			if ( !id ) {
				throw( XSError( "Failed to create framebuffer" ) );
			}
		}

		Framebuffer::~Framebuffer() {
			glDeleteFramebuffers( 1, &id );
		}

		void Framebuffer::AttachColorTexture( const Texture *texture, unsigned int slot ) {
			if ( slot >= MAX_FBO_COLOR_TEXTURES ) {
				console.Print( "Invalid slot number given (%d), valid range is 0 - %d", slot, MAX_FBO_COLOR_TEXTURES-1 );
				return;
			}

			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, texture->id, 0 );
			colorTextures[slot] = texture;
		}

		void Framebuffer::AttachDepthTexture( const Texture *texture ) {
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->id, 0 );
			depthTexture = texture;
		}

		void Framebuffer::AttachDepthStencilTexture( const Texture *texture ) {
			glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->id, 0 );
			depthTexture = texture;
			stencilTexture = texture->id;
		}

		void Framebuffer::Bind( void ) const {
			if ( currentReadFramebuffer != this || currentWriteFramebuffer != this ) {
				glBindFramebuffer( GL_FRAMEBUFFER, id );
				currentReadFramebuffer	= this;
				currentWriteFramebuffer	= this;
			}
		}

		void Framebuffer::Check( void ) const {
			unsigned int status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

			switch ( status ) {
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				console.Print( "One or more framebuffer attachment points are not complete.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				console.Print( "Invalid framebuffer attachment object type used.\n" );
				break;

			case GL_FRAMEBUFFER_UNSUPPORTED:
				console.Print( "More than one internal format was used in the color attachments.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				console.Print( "Missing a read buffer.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				console.Print( "No images were attached to the framebuffer.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				console.Print( "Number of samples is not the same for all rendertargets and color attachments.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
				console.Print( "Not all layered attachments are valid.\n" );
				break;

			case GL_FRAMEBUFFER_COMPLETE:
				break;
			}

			if ( status != GL_FRAMEBUFFER_COMPLETE ) {
				console.Print( "Creation of framebuffer %d could not be completed.\n", id );
			}
		}

	} // namespace Renderer

} // namespace XS
