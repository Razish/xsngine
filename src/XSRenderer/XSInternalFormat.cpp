#include "GLee/GLee.h"
#include "SDL2/SDL.h"

#include "XSRenderer/XSInternalFormat.h"

namespace XS {

	namespace Renderer {

		unsigned int GetGLInternalFormat( internalFormat_t format ) {
			switch ( format ) {
			default:
			case IF_RGBA8:
				return GL_RGBA8;

			case IF_RGBA16F:
				return GL_RGBA16F_ARB;

			case IF_DEPTH_COMPONENT16:
				return GL_DEPTH_COMPONENT16;

			case IF_DEPTH_COMPONENT24:
				return GL_DEPTH_COMPONENT24;

			case IF_STENCIL_INDEX4:
				return GL_STENCIL_INDEX4_EXT;

			case IF_STENCIL_INDEX8:
				return GL_STENCIL_INDEX8_EXT;

			case IF_DEPTH24_STENCIL8:
				return GL_DEPTH24_STENCIL8_EXT;
			}
		}

		unsigned int GetGLFormat( internalFormat_t format ) {
			switch ( format ) {
			default:
			case IF_RGBA8:
			case IF_RGBA16F:
				return GL_RGBA;

			case IF_DEPTH_COMPONENT16:
			case IF_DEPTH_COMPONENT24:
				return GL_DEPTH_COMPONENT;

			case IF_STENCIL_INDEX4:
			case IF_STENCIL_INDEX8:
				return GL_STENCIL_INDEX;

			case IF_DEPTH24_STENCIL8:
				return GL_DEPTH_STENCIL_EXT;
			}
		}

		unsigned int GetDataTypeForFormat( internalFormat_t format ) {
			switch ( format ) {
			default:
				return GL_UNSIGNED_BYTE;

			case IF_RGBA16F:
				return GL_FLOAT;

			case IF_DEPTH24_STENCIL8:
				return GL_UNSIGNED_INT_24_8_EXT;
			}
		}

	} // namespace Renderer

} // namespace XS
