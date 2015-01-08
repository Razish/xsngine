#pragma once

#include "XSCommon/XSVector.h"
#include "XSRenderer/XSModel.h"

namespace XS {

	namespace Renderer {

		class Obj : public Model {

			bool	LoadMeshes	( void );
			void	Process		( Mesh *mesh );

		};

	} // namespace Renderer

} // namespace XS
