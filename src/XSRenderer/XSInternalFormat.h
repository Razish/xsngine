#pragma once

namespace Renderer {

	enum class InternalFormat {
		NONE = 0,
		R8,
		RGBA8,
		RGBA16F,
		Depth16,
		Depth24,
		Stencil4,
		Stencil8,
		Depth24Stencil8
	};

	// ???
	unsigned int GetGLInternalFormat(
		InternalFormat internalFormat
	);

	// ???
	unsigned int GetGLFormat(
		InternalFormat internalFormat
	);

	// retrieve the data type used to store texture colours
	unsigned int GetDataTypeForFormat(
		InternalFormat internalFormat
	);

} // namespace Renderer
