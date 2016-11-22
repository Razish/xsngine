#include "Common/Common.h"
#include "Common/Console.h"
#include "Common/Error.h"
#include "Common/Cvar.h"
#include "Renderer/Renderer.h"
#include "Renderer/Framebuffer.h"

namespace Renderer {

	const Framebuffer *Framebuffer::currentReadFramebuffer = nullptr;
	const Framebuffer *Framebuffer::currentWriteFramebuffer = nullptr;

	// public, static
	void Framebuffer::BindDefault( void ) {
		if ( currentReadFramebuffer || currentWriteFramebuffer ) {
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			currentReadFramebuffer = nullptr;
			currentWriteFramebuffer = nullptr;
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

	void Framebuffer::BlitColour( const Framebuffer *source, const Framebuffer *destination, int sourceWidth,
		int sourceHeight, int destWidth, int destHeight )
	{
		Blit( source, destination, sourceWidth, sourceHeight, destWidth, destHeight, GL_COLOR_BUFFER_BIT );
	}

	void Framebuffer::BlitColourAndDepth( const Framebuffer *source, const Framebuffer *destination,
		int sourceWidth, int sourceHeight, int destWidth, int destHeight )
	{
		Blit( source, destination, sourceWidth, sourceHeight, destWidth, destHeight,
			GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// instance functions
	Framebuffer::Framebuffer()
	: id( 0 ), depthTexture( nullptr ), stencilTexture( 0u )
	{
		glGenFramebuffers( 1, &id );
		if ( Check() ) {
			if ( r_debug->GetBool() ) {
				console.Print( PrintLevel::Normal, "Creation of framebuffer %d completed successfully.\n", id );
			}
		}

		if ( !id ) {
			throw( XSError( "Failed to create framebuffer" ) );
		}

		for ( int i = 0; i < MAX_FBO_COLOR_TEXTURES; i++ ) {
			colourTextures[i] = nullptr;
		}
	}

	Framebuffer::~Framebuffer() {
		glDeleteFramebuffers( 1, &id );
	}

	void Framebuffer::AttachColourTexture( const Texture *texture, unsigned int slot ) {
		// make sure it's bound
		if ( currentReadFramebuffer != this || currentWriteFramebuffer != this ) {
			console.Print( PrintLevel::Normal,
				"Attaching colour texture to FBO without it being bound, binding now\n"
			);
			Bind();
		}

		if ( slot >= MAX_FBO_COLOR_TEXTURES ) {
			console.Print( PrintLevel::Normal, "Invalid slot number given (%d), valid range is 0 - %d\n",
				slot,
				MAX_FBO_COLOR_TEXTURES - 1
			);
			return;
		}

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, texture->id, 0 );
		colourTextures[slot] = texture;
	}

	void Framebuffer::AttachDepthTexture( const Texture *texture ) {
		// make sure it's bound
		if ( currentReadFramebuffer != this || currentWriteFramebuffer != this ) {
			console.Print( PrintLevel::Normal,
				"Attaching depth texture to FBO without it being bound, binding now\n"
			);
			Bind();
		}

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->id, 0 );
		depthTexture = texture;
	}

	void Framebuffer::AttachDepthStencilTexture( const Texture *texture ) {
		// make sure it's bound
		if ( currentReadFramebuffer != this || currentWriteFramebuffer != this ) {
			console.Print( PrintLevel::Normal, "Attaching depth/stencil texture to FBO without it being bound, "
				"binding now\n"
			);
			Bind();
		}

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->id, 0 );
		depthTexture = texture;
		stencilTexture = texture->id;
	}

	void Framebuffer::Bind( void ) const {
		if ( currentReadFramebuffer != this || currentWriteFramebuffer != this ) {
			glBindFramebuffer( GL_FRAMEBUFFER, id );
			currentReadFramebuffer	= this;
			currentWriteFramebuffer = this;
		}
	}

	bool Framebuffer::Check( void ) const {
		unsigned int status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

		switch ( status ) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
			console.Print( PrintLevel::Normal, "One or more framebuffer attachment points are not complete.\n" );
		} break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: {
			console.Print( PrintLevel::Normal, "Invalid framebuffer attachment object type used.\n" );
		} break;

		case GL_FRAMEBUFFER_UNSUPPORTED: {
			console.Print( PrintLevel::Normal,
				"More than one internal format was used in the colour attachments.\n"
			);
		} break;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: {
			console.Print( PrintLevel::Normal, "Missing a read buffer.\n" );
		} break;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
			console.Print( PrintLevel::Normal, "No images were attached to the framebuffer.\n" );
		} break;

		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: {
			console.Print( PrintLevel::Normal,
				"Number of samples is not the same for all rendertargets and colour attachments.\n"
			);
		} break;

		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: {
			console.Print( PrintLevel::Normal, "Not all layered attachments are valid.\n" );
		} break;

		case GL_FRAMEBUFFER_COMPLETE: {
			// this is the only acceptable case
			return true;
		} break;
		}

		if ( status != GL_FRAMEBUFFER_COMPLETE ) {
			console.Print( PrintLevel::Normal, "Creation of framebuffer %d could not be completed.\n", id );
		}

		return false;
	}

} // namespace Renderer
