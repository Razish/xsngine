#pragma once

#include <vector>

namespace XS {

	namespace Renderer {

		namespace Backend {
			class Buffer;
		} // namespace Backend

		class ShaderProgram;
		class Texture;

		struct Material {
			struct SamplerBinding {
				int		 unit;
				Texture	*texture;
			};

			struct BufferBinding {
				int				 index;
				Backend::Buffer	*buffer;
			};

			ShaderProgram				*shaderProgram;
			std::vector<SamplerBinding>	 samplerBindings;
			std::vector<BufferBinding>	 bufferBindings;

			// use this material for subsequent rendering
			void Bind(
				void
			) const;
		};

	} // namespace Renderer

} // namespace XS
