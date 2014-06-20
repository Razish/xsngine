#include "XSCommon/XSCommon.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSInternalFormat.h"

namespace XS {

	namespace Renderer {

		unsigned int GetGLInternalFormat( InternalFormat format ) {
			switch ( format ) {
			default:
			case InternalFormat::RGBA8:
				return GL_RGBA8;

			case InternalFormat::R8:
				return GL_R8;

			case InternalFormat::RGBA16F:
				return GL_RGBA16F_ARB;

			case InternalFormat::DEPTH_COMPONENT16:
				return GL_DEPTH_COMPONENT16;

			case InternalFormat::DEPTH_COMPONENT24:
				return GL_DEPTH_COMPONENT24;

			case InternalFormat::STENCIL_INDEX4:
				return GL_STENCIL_INDEX4_EXT;

			case InternalFormat::STENCIL_INDEX8:
				return GL_STENCIL_INDEX8_EXT;

			case InternalFormat::DEPTH24_STENCIL8:
				return GL_DEPTH24_STENCIL8_EXT;
			}
		}

		unsigned int GetGLFormat( InternalFormat format ) {
			switch ( format ) {
			default:
			case InternalFormat::RGBA8:
			case InternalFormat::RGBA16F:
				return GL_RGBA;

			case InternalFormat::R8:
				return GL_RED;

			case InternalFormat::DEPTH_COMPONENT16:
			case InternalFormat::DEPTH_COMPONENT24:
				return GL_DEPTH_COMPONENT;

			case InternalFormat::STENCIL_INDEX4:
			case InternalFormat::STENCIL_INDEX8:
				return GL_STENCIL_INDEX;

			case InternalFormat::DEPTH24_STENCIL8:
				return GL_DEPTH_STENCIL_EXT;
			}
		}

		unsigned int GetDataTypeForFormat( InternalFormat format ) {
			switch ( format ) {
			default:
				return GL_UNSIGNED_BYTE;

			case InternalFormat::RGBA16F:
				return GL_FLOAT;

			case InternalFormat::DEPTH24_STENCIL8:
				return GL_UNSIGNED_INT_24_8_EXT;
			}
		}

	} // namespace Renderer

} // namespace XS
