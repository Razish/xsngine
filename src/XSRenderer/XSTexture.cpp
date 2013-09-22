#include "XSSystem/XSInclude.h"

#include "GLee/GLee.h"
#include "SDL2/SDL.h"

#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Renderer {

		static int numTextures = 0;
		static texture_t textures[MAX_TEXTURES] = { {0} };

		//TODO: filter lookup for Texture_Create
		static const GLint filterTable[] = {
			GL_NEAREST,
			GL_LINEAR,
			GL_LINEAR_MIPMAP_LINEAR
		};

		void Texture_Cleanup( void ) {
			texture_t *texture = NULL;
			int i = 0;

			Print( "Cleaning up textures\n" );

			for ( i=0, texture=textures; i<numTextures; i++, texture++ ) {
				if ( !texture->id )
					continue;

				glDeleteTextures( 1, &texture->id );
				CheckGLErrors( __FILE__, __LINE__ );
			}

			numTextures = 0;
			memset( textures, 0, sizeof( textures ) );
		}

		texture_t *Texture_Create( unsigned int width, unsigned int height, internalFormat_t internalFormat, unsigned int minFilter, unsigned int magFilter ) {
			unsigned int textureId = 0;
			texture_t *texture = NULL;

			if ( numTextures >= MAX_TEXTURES ) {
				Print( "Exceeded maximum number of textures.\n" );
				return NULL;
			}

			glGenTextures( 1, &textureId );
			if ( !textureId ) {
				Print( String::Format( "Failed to create texture with internal ID %d.\n", numTextures ) );
				return NULL;
			}

			texture = &textures[numTextures++];
			texture->id = textureId;
			texture->width = width;
			texture->height = height;

			glBindTexture( GL_TEXTURE_2D, textureId );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			if ( r_textureFilterAnisotropic->GetInt() )
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLfloat)glConfig.maxAnisotropy );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterTable[minFilter] );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterTable[magFilter] );

			glTexImage2D( GL_TEXTURE_2D, 0, GetGLInternalFormat( internalFormat ), width, height, 0, GetGLFormat( internalFormat ), GetDataTypeForFormat( internalFormat ), NULL );
			glBindTexture( GL_TEXTURE_2D, 0 );

			CheckGLErrors (__FILE__, __LINE__);

			return texture;
		}

		texture_t *Texture_CreateBlank( unsigned int width, unsigned int height, internalFormat_t internalFormat ) {
			unsigned int textureId=0;
			unsigned int filterMode = GL_LINEAR_MIPMAP_LINEAR;
			texture_t *texture = NULL;

			if ( numTextures >= MAX_TEXTURES )
				throw( "CreateBlankTexture: Exceeded maximum number of textures" );

			glGenTextures( 1, &textureId );
			if ( !textureId )
				throw( String::Format( "CreateBlankTexture: Failed to create texture with internal ID %d", numTextures ) );

			texture			= &textures[numTextures++];
			texture->id		= textureId;
			texture->width	= width;
			texture->height	= height;

			// fp16 textures don't play nicely with filtering (performance, support)
			//	if ( internalFormat == IF_RGBA16F )
			//		filterMode = GL_NEAREST;

			glBindTexture( GL_TEXTURE_2D, textureId );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			if ( r_textureFilterAnisotropic->GetInt() )
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, (GLfloat)glConfig.maxAnisotropy );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode );

			glTexImage2D( GL_TEXTURE_2D, 0, GetGLInternalFormat( internalFormat ), width, height, 0, GetGLFormat( internalFormat ), GetDataTypeForFormat( internalFormat ), NULL );

			glBindTexture( GL_TEXTURE_2D, 0 );

			CheckGLErrors( __FILE__, __LINE__ );

			return texture;
		}

	}

}
