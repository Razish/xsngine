#pragma once

#include <vector>
#include "XSCommon/XSVector.h"
#include "XSRenderer/XSMaterial.h"

namespace XS {

	namespace Renderer {

		struct Mesh {

			vector3 *vertices;
			size_t numVertices;
			vector2 *uv;
			size_t numUVs;
			vector3 *normals;
			size_t numNormals;

			std::vector<uint16_t> vertexIndices;
			std::vector<uint16_t> uvIndices;
			std::vector<uint16_t> normalIndices;

			Material material;

		};

	} // namespace Renderer

} // namespace XS
