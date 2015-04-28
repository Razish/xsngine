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
				SamplerBinding(
					Texture *texture,
					const char *uniform = nullptr,
					int unit = 0
				);

				Texture		*texture;
				char		 uniform[128];
				int		 	 unit;
			};

			struct BufferBinding {
				BufferBinding()
				: index( 0 ), buffer( nullptr )
				{
				}

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
