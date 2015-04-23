#pragma once

#include <string>
#include <vector>

namespace XS {

	namespace Renderer {

		namespace Backend {
			class Buffer;
		} // namespace Backend

		class ShaderProgram;
		class Texture;

		const uint32_t MF_NONE		= 0x00000000u;
		const uint32_t MF_WIREFRAME	= 0x00000001u;

		struct Material {
			struct SamplerBinding {
				int		 	 unit;
				std::string	 uniform;
				Texture		*texture;
			};

			struct BufferBinding {
				int				 index;
				Backend::Buffer	*buffer;
			};

			ShaderProgram				*shaderProgram;
			std::vector<SamplerBinding>	 samplerBindings;
			std::vector<BufferBinding>	 bufferBindings;
			uint32_t					 flags;

			Material()
			: shaderProgram( nullptr ), flags( 0u )
			{
			}

			// use this material for subsequent rendering
			void Bind(
				void
			) const;
		};

	} // namespace Renderer

} // namespace XS
