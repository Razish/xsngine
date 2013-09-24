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

		static Cvar *r_textureAnisotropy;
		static Cvar *r_textureAnisotropyMax;
		static Cvar *r_textureFilter;

		static bool anisotropy;
		static float maxAnisotropy;

		static std::unordered_map<const char *, GLint> filterTable;

		void Texture::Init( void ) {
			filterTable["GL_NEAREST"] = GL_NEAREST;
			filterTable["GL_LINEAR"] = GL_LINEAR;
			filterTable["GL_LINEAR_MIPMAP_LINEAR"] = GL_LINEAR_MIPMAP_LINEAR;

			r_textureAnisotropy = Cvar::Create( "r_textureAnisotropy", "1", Cvar::ARCHIVE );
			r_textureAnisotropyMax = Cvar::Create( "r_textureAnisotropyMax", "16.0", Cvar::ARCHIVE );

			// get anisotropic filtering settings
			if ( SDL_GL_ExtensionSupported( "GL_EXT_texture_filter_anisotropic" ) )
				anisotropy = true;
			glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
		}

		Texture::Texture( unsigned int width, unsigned int height, internalFormat_t internalFormat ) {
			GLint filterMode = filterTable[r_textureFilter->GetCString()];
			if ( !filterMode )
				filterMode = GL_LINEAR_MIPMAP_LINEAR;

			glGenTextures( 1, &id );
			if ( !id )
				throw( "Failed to create blank texture" );

			this->width		= width;
			this->height	= height;

			// fp16 textures don't play nicely with filtering (performance, support)
			//	if ( internalFormat == IF_RGBA16F )
			//		filterMode = GL_NEAREST;

			glBindTexture( GL_TEXTURE_2D, id );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			if ( anisotropy && r_textureAnisotropy->GetBool() )
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, std::min( r_textureAnisotropyMax->GetFloat(), maxAnisotropy ) );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode );

			glTexImage2D( GL_TEXTURE_2D, 0, GetGLInternalFormat( internalFormat ), width, height, 0, GetGLFormat( internalFormat ), GetDataTypeForFormat( internalFormat ), NULL );
			glBindTexture( GL_TEXTURE_2D, 0 );

			CheckGLErrors( __FILE__, __LINE__ );
		}

		Texture::~Texture() {
			glDeleteTextures( 1, &id );
			CheckGLErrors( __FILE__, __LINE__ );
		}

	}

}
