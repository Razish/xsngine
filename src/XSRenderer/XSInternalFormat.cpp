#include "GLee/GLee.h"
#include "SDL2/SDL.h"

#include "XSRenderer/XSInternalFormat.h"

namespace XS {

	namespace Renderer {

		unsigned int GetGLInternalFormat( InternalFormat internalFormat ) {
			switch ( internalFormat ) {
			default:
			case RGBA8:
				return GL_RGBA8;

			case RGBA16F:
				return GL_RGBA16F_ARB;

			case DEPTH_COMPONENT16:
				return GL_DEPTH_COMPONENT16;

			case DEPTH_COMPONENT24:
				return GL_DEPTH_COMPONENT24;

			case STENCIL_INDEX4:
				return GL_STENCIL_INDEX4_EXT;

			case STENCIL_INDEX8:
				return GL_STENCIL_INDEX8_EXT;

			case DEPTH24_STENCIL8:
				return GL_DEPTH24_STENCIL8_EXT;
			}
		}

		unsigned int GetGLFormat( InternalFormat internalFormat ) {
			switch ( internalFormat ) {
			default:
			case RGBA8:
			case RGBA16F:
				return GL_RGBA;

			case DEPTH_COMPONENT16:
			case DEPTH_COMPONENT24:
				return GL_DEPTH_COMPONENT;

			case STENCIL_INDEX4:
			case STENCIL_INDEX8:
				return GL_STENCIL_INDEX;

			case DEPTH24_STENCIL8:
				return GL_DEPTH_STENCIL_EXT;
			}
		}

		unsigned int GetDataTypeForFormat( InternalFormat internalFormat ) {
			switch ( internalFormat ) {
			default:
				return GL_UNSIGNED_BYTE;

			case RGBA16F:
				return GL_FLOAT;

			case DEPTH24_STENCIL8:
				return GL_UNSIGNED_INT_24_8_EXT;
			}
		}

	} // namespace Renderer

} // namespace XS
