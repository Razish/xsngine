#include "XSCommon/XSCommon.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSInternalFormat.h"

namespace XS {

	namespace Renderer {

		unsigned int GetGLInternalFormat( InternalFormat format ) {
			switch ( format ) {
			default:
			case InternalFormat::RGBA8: {
				return GL_RGBA8;
			} break;

			case InternalFormat::R8: {
				return GL_R8;
			} break;

			case InternalFormat::RGBA16F: {
				return GL_RGBA16F_ARB;
			} break;

			case InternalFormat::Depth16: {
				return GL_DEPTH_COMPONENT16;
			} break;

			case InternalFormat::Depth24: {
				return GL_DEPTH_COMPONENT24;
			} break;

			case InternalFormat::Stencil4: {
				return GL_STENCIL_INDEX4_EXT;
			} break;

			case InternalFormat::Stencil8: {
				return GL_STENCIL_INDEX8_EXT;
			} break;

			case InternalFormat::Depth24Stencil8: {
				return GL_DEPTH24_STENCIL8_EXT;
			} break;
			}
		}

		unsigned int GetGLFormat( InternalFormat internalFormat ) {
			switch ( internalFormat ) {
			default:
			case InternalFormat::RGBA8:
			case InternalFormat::RGBA16F: {
				return GL_RGBA;
			} break;

			case InternalFormat::R8: {
				return GL_RED;
			} break;

			case InternalFormat::Depth16:
			case InternalFormat::Depth24: {
				return GL_DEPTH_COMPONENT;
			} break;

			case InternalFormat::Stencil4:
			case InternalFormat::Stencil8: {
				return GL_STENCIL_INDEX;
			} break;

			case InternalFormat::Depth24Stencil8: {
				return GL_DEPTH_STENCIL_EXT;
			} break;
			}
		}

		unsigned int GetDataTypeForFormat( InternalFormat internalFormat ) {
			switch ( internalFormat ) {
			default: {
				return GL_UNSIGNED_BYTE;
			} break;

			case InternalFormat::RGBA16F: {
				return GL_FLOAT;
			} break;

			case InternalFormat::Depth24Stencil8: {
				return GL_UNSIGNED_INT_24_8_EXT;
			} break;
			}
		}

	} // namespace Renderer

} // namespace XS
