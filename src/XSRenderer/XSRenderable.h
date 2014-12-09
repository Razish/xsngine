#pragma once

#include "XSRenderer/XSMesh.h"

namespace XS {

	namespace Renderer {

		// abstract base class implemented by e.g. Model
		class Renderable {

		public:
			virtual ~Renderable() = 0;

		};

	} // namespace Renderer

} // namespace XS
