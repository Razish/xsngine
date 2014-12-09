#pragma once

#include "XSRenderer/XSBuffer.h"

namespace XS {

	namespace Renderer {

		class ShaderProgram;
		class Texture;

		struct Material {
			struct SamplerBinding {
				int		unit;
				Texture	*texture;
			};

			struct BufferBinding {
				int				index;
				Backend::Buffer	*buffer;
			};

			ShaderProgram *shaderProgram;
			std::vector<SamplerBinding> samplerBindings;
			std::vector<BufferBinding> bufferBindings;

			void Bind( void ) const;
		};

	} // namespace Renderer

} // namespace XS
