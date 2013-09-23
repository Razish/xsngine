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

		static std::vector<Texture*> textures;

		//TODO: filter lookup for Texture_Create
		static const GLint filterTable[] = {
			GL_NEAREST,
			GL_LINEAR,
			GL_LINEAR_MIPMAP_LINEAR
		};

		void Texture::Init( void ) {
			if ( SDL_GL_ExtensionSupported( "GL_EXT_texture_filter_anisotropic" ) )
				glConfig.supports.anisotropy = true;
		}

		void Texture::Cleanup( void ) {
			Print( "Cleaning up textures\n" );

			for ( auto it = textures.begin(); it != textures.end(); ++it ) {
				if ( !(*it)->id )
					continue;

				glDeleteTextures( 1, &(*it)->id );

				delete *it;
				CheckGLErrors( __FILE__, __LINE__ );
			}

			textures.clear();
		}

		Texture::Texture( unsigned int width, unsigned int height, internalFormat_t internalFormat, unsigned int minFilter, unsigned int magFilter ) {
			unsigned int textureId = 0;

			glGenTextures( 1, &textureId );
			if ( !textureId )
				throw( String::Format( "Failed to create texture with internal ID %d.\n", textures.size() ) );

			this->id = textureId;
			this->width = width;
			this->height = height;

			glBindTexture( GL_TEXTURE_2D, textureId );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			if ( glConfig.supports.anisotropy && r_textureAnisotropy->GetBool() )
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min( r_textureAnisotropyMax->GetFloat(), glConfig.maxAnisotropy ) );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterTable[minFilter] );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterTable[magFilter] );

			glTexImage2D( GL_TEXTURE_2D, 0, GetGLInternalFormat( internalFormat ), width, height, 0, GetGLFormat( internalFormat ), GetDataTypeForFormat( internalFormat ), NULL );
			glBindTexture( GL_TEXTURE_2D, 0 );

			CheckGLErrors (__FILE__, __LINE__);

			textures.push_back( this );
		}

		Texture::Texture( unsigned int width, unsigned int height, internalFormat_t internalFormat ) {
			unsigned int textureId=0;
			unsigned int filterMode = GL_LINEAR_MIPMAP_LINEAR;

			glGenTextures( 1, &textureId );
			if ( !textureId )
				throw( String::Format( "Failed to create blank texture with internal ID %d", textures.size() ) );

			this->id		= textureId;
			this->width		= width;
			this->height	= height;

			// fp16 textures don't play nicely with filtering (performance, support)
			//	if ( internalFormat == IF_RGBA16F )
			//		filterMode = GL_NEAREST;

			glBindTexture( GL_TEXTURE_2D, textureId );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			if ( glConfig.supports.anisotropy && r_textureAnisotropy->GetBool() )
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min( r_textureAnisotropyMax->GetFloat(), glConfig.maxAnisotropy ) );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode );

			glTexImage2D( GL_TEXTURE_2D, 0, GetGLInternalFormat( internalFormat ), width, height, 0, GetGLFormat( internalFormat ), GetDataTypeForFormat( internalFormat ), NULL );

			glBindTexture( GL_TEXTURE_2D, 0 );

			CheckGLErrors( __FILE__, __LINE__ );

			textures.push_back( this );
		}

	}

}
