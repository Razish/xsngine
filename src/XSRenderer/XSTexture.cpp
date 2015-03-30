#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSError.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSTexture.h"

namespace XS {

	namespace Renderer {

		static Cvar *r_textureAnisotropy = nullptr;
		static Cvar *r_textureAnisotropyMax = nullptr;
		static Cvar *r_textureFilter = nullptr;

		static bool anisotropy = false;
		static real32_t maxAnisotropy = 0.0f;

		static const struct TextureFilter {
			const char *name;
			GLint min, mag;
			bool generateMipMaps;
		} filterTable[] = {
			{ "GL_NEAREST",					GL_NEAREST,					GL_NEAREST,	false },
			{ "GL_LINEAR",					GL_LINEAR,					GL_LINEAR,	false },
			{ "GL_NEAREST_MIPMAP_NEAREST",	GL_NEAREST_MIPMAP_NEAREST,	GL_NEAREST,	true },
			{ "GL_LINEAR_MIPMAP_NEAREST",	GL_LINEAR_MIPMAP_NEAREST,	GL_LINEAR,	true },
			{ "GL_NEAREST_MIPMAP_LINEAR",	GL_NEAREST_MIPMAP_LINEAR,	GL_NEAREST,	true },
			{ "GL_LINEAR_MIPMAP_LINEAR",	GL_LINEAR_MIPMAP_LINEAR,	GL_LINEAR,	true }
		};
		static const TextureFilter &GetTextureFilter( const char *string ) {
			for ( const TextureFilter &filter : filterTable ) {
				if ( !String::Compare( string, filter.name ) ) {
					return filter;
				}
			}
			return GetTextureFilter( r_textureFilter->GetDefaultString().c_str() );
		}

		const Texture *Texture::lastUsedTexture[maxTextureUnits] = {};
		int Texture::lastUsedTextureUnit = 0;

		void Texture::Init( void ) {
			r_textureAnisotropy = Cvar::Create( "r_textureAnisotropy", "1", "Enable anisotropic filtering", CVAR_ARCHIVE );
			r_textureAnisotropyMax = Cvar::Create( "r_textureAnisotropyMax", "16.0", "Anisotropic filtering level",
				CVAR_ARCHIVE );
			r_textureFilter = Cvar::Create( "r_textureFilter", "GL_LINEAR_MIPMAP_LINEAR", "Texture filtering mode",
				CVAR_ARCHIVE );

			// get anisotropic filtering settings
			if ( GLEW_EXT_texture_filter_anisotropic ) {
				anisotropy = true;
				glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy );
			}
		}

		Texture::Texture( uint32_t width, uint32_t height, InternalFormat internalFormat, const uint8_t *data ) {
			glGenTextures( 1, &id );
			if ( !id ) {
				throw( XSError( "Failed to create blank texture" ) );
			}

			Bind( 0 );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

			if ( anisotropy && r_textureAnisotropy->GetBool() ) {
				glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
					std::min( r_textureAnisotropyMax->GetReal32(), maxAnisotropy ) );
			}

			const TextureFilter &filterMode = GetTextureFilter( r_textureFilter->GetCString() );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode.min );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode.mag );

			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

			glTexImage2D( GL_TEXTURE_2D, 0, GetGLInternalFormat( internalFormat ), width, height, 0,
				GetGLFormat( internalFormat ), GetDataTypeForFormat( internalFormat ), data );

			if ( filterMode.min == GL_NEAREST_MIPMAP_LINEAR ||
				filterMode.min == GL_NEAREST_MIPMAP_NEAREST ||
				filterMode.min == GL_LINEAR_MIPMAP_LINEAR ||
				filterMode.min == GL_LINEAR_MIPMAP_NEAREST )
			{
				glGenerateMipmap( GL_TEXTURE_2D );
			}
 		}

		Texture::~Texture() {
			glDeleteTextures( 1, &id );
		}

		void Texture::Bind( int unit ) const {
			if ( lastUsedTextureUnit != unit ) {
				glActiveTexture( GL_TEXTURE0 + unit );
				lastUsedTextureUnit = unit;
			}

			if ( lastUsedTexture[unit] != this ) {
				glBindTexture( GL_TEXTURE_2D, id );
			}
		}

	} // namespace Renderer

} // namespace XS
