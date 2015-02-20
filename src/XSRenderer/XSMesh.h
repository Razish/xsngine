#pragma once

#include <vector>
#include "XSCommon/XSVector.h"

namespace XS {

	namespace Renderer {

		struct Material;
		class ShaderProgram;
		class Texture;

		namespace Backend {
			class Buffer;
		} // namespace Backend

		struct Mesh {

			std::vector<vector3>	 vertices;
			std::vector<vector3>	 normals;
			std::vector<vector2>	 UVs;
			Backend::Buffer			*vertexBuffer;

			std::vector<uint16_t>	 indices;
			Backend::Buffer			*indexBuffer;

			ShaderProgram			*shader;
			Material				*material;
			Texture					*texture;

			// upload mesh to GPU
			void Upload(
				void
			);

			Mesh()
			: vertexBuffer( nullptr ), indexBuffer( nullptr ), shader( nullptr ), material( nullptr ),
				texture( nullptr )
			{
			}
			~Mesh();

		};

	} // namespace Renderer

} // namespace XS
