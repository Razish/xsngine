#pragma once

#include "XSCommon/XSVector.h"
#include "XSRenderer/XSMaterial.h"

namespace XS {

	namespace Renderer {

		struct Mesh {

			vector3 *vertices;
			vector2 *texCoords;

			Material material;

		};

	} // namespace Renderer

} // namespace XS
