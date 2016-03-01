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

			bool					 uploaded = false;
			bool					 materialCreated = false;

			std::vector<vector3>	 vertices;
			std::vector<vector3>	 normals;
			std::vector<vector2>	 UVs;
			std::vector<uint32_t>	 indices;

			Backend::Buffer			*vertexBuffer = nullptr;
			Backend::Buffer			*indexBuffer = nullptr;

			ShaderProgram			*shader = nullptr;
			Material				*material = nullptr;
			Texture					*texture = nullptr;

			// create material before uploading
			void CreateMaterial(
				void
			);

			// upload mesh to GPU
			void Upload(
				void
			);

			~Mesh();

		};

	} // namespace Renderer

} // namespace XS
