#include "System/XSInclude.h"

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

		#define MAX_FRAMEBUFFERS	(32)
		#define MAX_RENDERBUFFERS	(2 * MAX_FRAMEBUFFERS)

		static int numFramebuffers = 0;
		static framebuffer_t framebuffers[MAX_FRAMEBUFFERS] = { {0} };

		static unsigned int numRenderbuffers = 0;
		static unsigned int renderbuffers[MAX_RENDERBUFFERS] = { 0 };

		static const framebuffer_t *currentReadFramebuffer	= NULL;
		static const framebuffer_t *currentWriteFramebuffer	= NULL;

		static bool initialised = false;

		void Framebuffer_Init( void ) {
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

		void Framebuffer_Cleanup( void ) {
			framebuffer_t *fbo = NULL;
			int i = 0;

			Print( "Cleaning up framebuffers\n" );

			for ( i=0, fbo=framebuffers; i<numFramebuffers; i++, fbo++ ) {
				if ( !fbo->id )
					continue;

				glDeleteFramebuffersEXT( 1, &fbo->id );
				CheckGLErrors( __FILE__, __LINE__ );
			}

			glDeleteRenderbuffersEXT( numRenderbuffers, renderbuffers );

			currentReadFramebuffer = currentWriteFramebuffer = NULL;
			numFramebuffers = numRenderbuffers = 0;
			memset( framebuffers, 0, sizeof( framebuffers ) );
			memset( renderbuffers, 0, sizeof( renderbuffers ) );

			initialised = false;
		}

		void Framebuffer_Check( const framebuffer_t *framebuffer ) {
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
				Print( String::Format( "Creation of framebuffer %d could not be completed.\n", framebuffer->id ) );

			CheckGLErrors( __FILE__, __LINE__ );
		}

		framebuffer_t *Framebuffer_Create( void ) {
			framebuffer_t *fbo = NULL;

			if ( !initialised )
				throw( "CreateFramebuffer: Framebuffer extension not initialised" );

			if ( numFramebuffers >= MAX_FRAMEBUFFERS )
				throw( "CreateFramebuffer: Maximum number of framebuffers exeeded" );

			fbo = &framebuffers[numFramebuffers];
			glGenFramebuffersEXT( 1, &fbo->id );

			if ( !fbo->id )
				throw( String::Format( "CreateFramebuffer: Failed to create framebuffer with internal ID %d", numFramebuffers ) );

			numFramebuffers++;

			CheckGLErrors( __FILE__, __LINE__ );

			return fbo;
		}

		void Framebuffer_AttachColorTexture( framebuffer_t *framebuffer, const texture_t *texture, unsigned int slot ) {
			if ( slot >= MAX_FBO_COLOR_TEXTURES ) {
				Print( String::Format( "Invalid slot number given (%d), valid range is 0 - %d", slot, MAX_FBO_COLOR_TEXTURES-1 ) );
				return;
			}

			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + slot, GL_TEXTURE_2D, texture->id, 0 );
			framebuffer->colorTextures[slot] = texture;

			CheckGLErrors( __FILE__, __LINE__ );
		}

#if 0 //BROKEN
		void Framebuffer_AttachDepthRenderbuffer( framebuffer_t *framebuffer, unsigned int renderbufferId )
		{
			glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderbufferId );
			framebuffer->depthTexture = renderbufferId;

			CheckGLErrors( __FILE__, __LINE__ );
		}
#endif

		void Framebuffer_AttachDepthTexture( framebuffer_t *framebuffer, const texture_t *texture ) {
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, texture->id, 0 );
			framebuffer->depthTexture = texture;//texture->id;

			CheckGLErrors( __FILE__, __LINE__ );
		}

		void Framebuffer_AttachDepthStencilTexture( framebuffer_t *framebuffer, const texture_t *texture ) {
			glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->id, 0 );
			framebuffer->depthTexture = texture;//texture->id;
			framebuffer->stencilTexture = texture->id;

			CheckGLErrors( __FILE__, __LINE__ );
		}

		void Framebuffer_AttachStencilRenderbuffer( framebuffer_t *framebuffer, unsigned int renderbufferId ) {
			glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderbufferId );
			framebuffer->stencilTexture = renderbufferId;

			CheckGLErrors( __FILE__, __LINE__ );
		}

		void Framebuffer_BindDefault( void ) {
			if ( currentReadFramebuffer || currentWriteFramebuffer ) {
				glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
				currentReadFramebuffer	= NULL;
				currentWriteFramebuffer	= NULL;
			}

			CheckGLErrors( __FILE__, __LINE__ );
		}

		void Framebuffer_Bind( const framebuffer_t *framebuffer ) {
			if ( currentReadFramebuffer != framebuffer || currentWriteFramebuffer != framebuffer ) {
				glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, framebuffer->id );
				currentReadFramebuffer	= framebuffer;
				currentWriteFramebuffer	= framebuffer;
			}

			CheckGLErrors( __FILE__, __LINE__ );
		}

		const framebuffer_t *Framebuffer_GetCurrent( void ) {
			return currentReadFramebuffer;
		}

		void Framebuffer_Blit( const framebuffer_t *source, const framebuffer_t *destination, int sourceWidth, int sourceHeight, int destWidth, int destHeight, unsigned int bufferBits ) {
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

		void Framebuffer_BlitColor( const framebuffer_t *source, const framebuffer_t *destination, int sourceWidth, int sourceHeight, int destWidth, int destHeight ) {
			Framebuffer_Blit( source, destination, sourceWidth, sourceHeight, destWidth, destHeight, GL_COLOR_BUFFER_BIT);
		}

		void Framebuffer_BlitColorAndDepth( const framebuffer_t *source, const framebuffer_t *destination, int sourceWidth, int sourceHeight, int destWidth, int destHeight ) {
			Framebuffer_Blit( source, destination, sourceWidth, sourceHeight, destWidth, destHeight, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		}

		unsigned int Framebuffer_CreateRenderbuffer( unsigned int width, unsigned int height, internalFormat_t internalFormat ) {
			unsigned int renderbufferId = 0;

			if ( numRenderbuffers >= MAX_RENDERBUFFERS )
				throw( "CreateRenderbuffer: Exceeded maximum number of renderbuffers" );

			glGenRenderbuffersEXT( 1, &renderbufferId );
			if ( !renderbufferId )
				throw( String::Format( "CreateRenderbuffer: Failed to create renderbuffer with internal ID %d", numRenderbuffers ) );

			glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, renderbufferId );
			glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GetGLInternalFormat( internalFormat ), width, height );

			renderbuffers[numRenderbuffers++] = renderbufferId;

			CheckGLErrors( __FILE__, __LINE__ );

			return renderbufferId;
		}

	}

}
