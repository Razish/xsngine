#include "XSSystem/XSInclude.h"

#include "GLee/GLee.h"
#include "SDL2/SDL.h"

#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSFramebuffer.h"

namespace XS {

	namespace Renderer {

		static Cvar *r_fbo;
		const Framebuffer *Framebuffer::currentReadFramebuffer = NULL;
		const Framebuffer *Framebuffer::currentWriteFramebuffer = NULL;

		static const char *extensionsRequired[] = {
			"GL_EXT_framebuffer_object",
			"GL_EXT_framebuffer_blit",
		};
		static const size_t numExtensionsRequired = ARRAY_LEN( extensionsRequired );

		void Framebuffer::Init( void ) {
			r_fbo = Cvar::Create( "r_fbo", "1", CVAR_ARCHIVE );

			// let them disable GLSL entirely
			if ( !r_fbo->GetBool() ) {
				Console::Print( "Not using framebuffer extension\n" );
				return;
			}

			bool supported = true;
			for ( size_t i=0; i<numExtensionsRequired; i++ ) {
				if ( !SDL_GL_ExtensionSupported( extensionsRequired[i] ) ) {
					supported = false;
					Console::Print( "Warning: Required OpenGL extension '%s' not available\n", extensionsRequired[i] );
				}
			}

			if ( supported )
				Console::Print( "Framebuffer extension loaded\n" );
			else
				Console::Print( "Framebuffer extension unavailable\n" );
			r_fbo->Set( supported );

			currentReadFramebuffer = currentWriteFramebuffer = NULL;
		}

		void Framebuffer::BindDefault( void ) {
			if ( currentReadFramebuffer || currentWriteFramebuffer ) {
				glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
				currentReadFramebuffer	= NULL;
				currentWriteFramebuffer	= NULL;
			}
		}

		const Framebuffer *Framebuffer::GetCurrent( void ) {
			return currentReadFramebuffer;
		}

		void Framebuffer::Blit( const Framebuffer *source, const Framebuffer *destination, int sourceWidth,
			int sourceHeight, int destWidth, int destHeight, unsigned int bufferBits )
		{
			if ( currentReadFramebuffer != source ) {
				glBindFramebufferEXT( GL_READ_FRAMEBUFFER_EXT, source ? source->id : 0 );
				currentReadFramebuffer = source;
			}

			if ( currentWriteFramebuffer != destination ) {
				glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER_EXT, destination ? destination->id : 0 );
				currentWriteFramebuffer = destination;
			}

			glBlitFramebufferEXT( sourceWidth, sourceHeight, 0, 0, destWidth, destHeight, 0, 0, bufferBits, GL_NEAREST );
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
		Framebuffer::Framebuffer() {
			id = 0;

			if ( !r_fbo->GetBool() )
				return;

			glGenFramebuffersEXT( 1, &id );

			if ( !id )
				throw( "Failed to create framebuffer" );
		}

		Framebuffer::~Framebuffer() {
			glDeleteFramebuffersEXT( 1, &id );
		}

		void Framebuffer::AttachColorTexture( const Texture *texture, unsigned int slot ) {
			if ( slot < 0 || slot >= MAX_FBO_COLOR_TEXTURES ) {
				Console::Print( "Invalid slot number given (%d), valid range is 0 - %d", slot, MAX_FBO_COLOR_TEXTURES-1 );
				return;
			}

			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + slot, GL_TEXTURE_2D, texture->id, 0 );
			colorTextures[slot] = texture;

		}

		void Framebuffer::AttachDepthTexture( const Texture *texture ) {
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, texture->id, 0 );
			depthTexture = texture;
		}

		void Framebuffer::AttachDepthStencilTexture( const Texture *texture ) {
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->id, 0 );
			depthTexture = texture;
			stencilTexture = texture->id;
		}

		void Framebuffer::Bind( void ) {
			if ( currentReadFramebuffer != this || currentWriteFramebuffer != this ) {
				glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, id );
				currentReadFramebuffer	= this;
				currentWriteFramebuffer	= this;
			}
		}

		void Framebuffer::Check( void ) {
			unsigned int status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );

			switch ( status ) {
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				Console::Print( "One or more framebuffer attachment points are not complete.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				Console::Print( "One or more attached images have different dimensions.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				Console::Print( "Invalid framebuffer attachment object type used.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				Console::Print( "More than one internal format was used in the color attachments.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				Console::Print( "Missing a read buffer.\n" );
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				Console::Print( "No images were attached to the framebuffer.\n" );
				break;

			case GL_FRAMEBUFFER_COMPLETE_EXT:
				break;
			}

			if ( status != GL_FRAMEBUFFER_COMPLETE_EXT )
				Console::Print( "Creation of framebuffer %d could not be completed.\n", id );
		}

	}

}
