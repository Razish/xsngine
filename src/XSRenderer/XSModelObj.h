#pragma once

#include "XSCommon/XSVector.h"
#include "XSRenderer/XSModel.h"

namespace XS {

	namespace Renderer {

		class Obj : public Model {

			// load the mesh from disk
			bool LoadMeshes(
				void
			);

			// process mesh into GPU-friendly format after loading the model format from disk
			void Process(
				Mesh *mesh
			);

		};

	} // namespace Renderer

} // namespace XS
