#pragma once

namespace XS {

	namespace Renderer {

		typedef enum internalFormat_e {
			IF_NONE=0,
			IF_RGBA8,
			IF_RGBA16F,
			IF_DEPTH_COMPONENT16,
			IF_DEPTH_COMPONENT24,
			IF_STENCIL_INDEX4,
			IF_STENCIL_INDEX8,
			IF_DEPTH24_STENCIL8
		} internalFormat_t;

		unsigned int GetGLInternalFormat( internalFormat_t internalFormat );
		unsigned int GetGLFormat( internalFormat_t internalFormat );
		unsigned int GetDataTypeForFormat( internalFormat_t internalFormat );

	}

}
