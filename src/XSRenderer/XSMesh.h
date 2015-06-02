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
			Backend::Buffer			*vertexBuffer = nullptr;

			std::vector<uint32_t>	 indices;
			Backend::Buffer			*indexBuffer = nullptr;

			ShaderProgram			*shader = nullptr;
			Material				*material = nullptr;
			Texture					*texture = nullptr;

			// upload mesh to GPU
			void Upload(
				void
			);

			~Mesh();
		};

	} // namespace Renderer

} // namespace XS
