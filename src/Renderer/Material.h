#pragma once

#include <string>
#include <vector>

#include "Renderer/Buffer.h"
#include "Renderer/Texture.h"

namespace Renderer {

	class ShaderProgram;

	const uint32_t MF_NONE		= 0x00000000u;
	const uint32_t MF_WIREFRAME	= 0x00000001u;

	struct Material {

		struct SamplerBinding {

			int		 	 	 unit;
			std::string	 	 uniform;
			const Texture	*texture;

		};

		struct BufferBinding {

			int				 index;
			Backend::Buffer	*buffer;

			~BufferBinding() {
				delete buffer;
				buffer = nullptr;
			}

		};

		ShaderProgram				*shaderProgram = nullptr;
		std::vector<SamplerBinding>	 samplerBindings;
		std::vector<BufferBinding>	 bufferBindings;
		uint32_t					 flags = 0u;

		// use this material for subsequent rendering
		void Bind(
			void
		) const;

	};

} // namespace Renderer
