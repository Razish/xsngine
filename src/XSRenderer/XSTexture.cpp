#include "XSSystem/XSInclude.h"
#include "XSSystem/XSPlatform.h"

#include <GL/glew.h>
#include "SDL2/SDL.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSError.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Renderer {

		static Cvar *r_textureAnisotropy, *r_textureAnisotropyMax, *r_textureFilter;

		static bool anisotropy;
		static float maxAnisotropy;

		static const struct {
			const char *name;
			GLint min, mag;
		} filterTable[] = {
			{ "GL_NEAREST",					GL_NEAREST,					GL_NEAREST },
			{ "GL_LINEAR",					GL_LINEAR,					GL_LINEAR },
			{ "GL_NEAREST_MIPMAP_NEAREST",	GL_NEAREST_MIPMAP_NEAREST,	GL_NEAREST },
			{ "GL_LINEAR_MIPMAP_NEAREST",	GL_LINEAR_MIPMAP_NEAREST,	GL_LINEAR },
			{ "GL_NEAREST_MIPMAP_LINEAR",	GL_NEAREST_MIPMAP_LINEAR,	GL_NEAREST },
			{ "GL_LINEAR_MIPMAP_LINEAR",	GL_LINEAR_MIPMAP_LINEAR,	GL_LINEAR }
		};
		static const size_t numFilters = ARRAY_LEN( filterTable );
		static size_t GetTextureFilter( const char *string ) {
			for ( size_t filter=0; filter<numFilters; filter++ ) {
				if ( !String::Compare( string, filterTable[filter].name ) )
					return filter;
			}
			return 0;
		}

		const Texture *Texture::lastUsedTexture[MAX_TEXTURE_UNITS] = {};
		int Texture::lastUsedTextureUnit = 0;

		void Texture::Init( void ) {
			r_textureAnisotropy = Cvar::Create( "r_textureAnisotropy", "1", "Enable anisotropic filtering", CVAR_ARCHIVE );
			r_textureAnisotropyMax = Cvar::Create( "r_textureAnisotropyMax", "16.0", "Anisotropic filtering level",
				CVAR_ARCHIVE );
			r_textureFilter = Cvar::Create( "r_textureFilter", "GL_LINEAR_MIPMAP_LINEAR", "Texture filtering mode",
				CVAR_ARCHIVE );

			// get anisotropic filtering settings
			if ( GLEW_EXT_texture_filter_anisotropic )
				anisotropy = true;
			glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
		}

		Texture::Texture( unsigned int width, unsigned int height, InternalFormat internalFormat, byte *data )
			: width( width ), height( height ) {
			size_t filterMode = GetTextureFilter( r_textureFilter->GetCString() );

			glGenTextures( 1, &id );
			if ( !id )
				throw( XSError( "Failed to create blank texture" ) );

			// fp16 textures don't play nicely with filtering (performance, support)
			//	if ( internalFormat == IF_RGBA16F )
			//		filterMode = GL_NEAREST;

			Bind (0);

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			if ( anisotropy && r_textureAnisotropy->GetBool() )
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
					std::min( r_textureAnisotropyMax->GetFloat(), maxAnisotropy ) );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterTable[filterMode].min );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterTable[filterMode].mag );

			glTexImage2D( GL_TEXTURE_2D, 0, GetGLInternalFormat( internalFormat ), width, height, 0,
				GetGLFormat( internalFormat ), GetDataTypeForFormat( internalFormat ), data );

			if ( filterTable[filterMode].min == GL_NEAREST_MIPMAP_LINEAR ||
				filterTable[filterMode].min == GL_NEAREST_MIPMAP_NEAREST ||
				filterTable[filterMode].min == GL_LINEAR_MIPMAP_LINEAR ||
				filterTable[filterMode].min == GL_LINEAR_MIPMAP_NEAREST )
			{
				glGenerateMipmap( GL_TEXTURE_2D );
			}
 		}

		Texture::~Texture() {
			glDeleteTextures( 1, &id );
		}

		void Texture::Bind (int unit) const {
			if ( lastUsedTextureUnit != unit ) {
				glActiveTexture (GL_TEXTURE0 + unit);
				lastUsedTextureUnit = unit;
			}

			if ( lastUsedTexture[unit] != this ) {
				glBindTexture (GL_TEXTURE_2D, id);
			}
		}

	} // namespace Renderer

} // namespace XS
