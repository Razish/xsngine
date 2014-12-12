#pragma once

#include "XSCommon/XSVector.h"
#include "XSRenderer/XSModel.h"

namespace XS {

	namespace Renderer {

		class Obj : public Model {

			bool	LoadMeshes	( void );
			void	Process		( Mesh *mesh, std::vector<vector3> &tmpVertices, std::vector<vector2> &tmpUVs,
				std::vector<vector3> &tmpNormals );

		};

	} // namespace Renderer

} // namespace XS
