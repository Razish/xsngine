#pragma once

#include "XSCommon/XSMatrix.h"

namespace XS {

	namespace Renderer {

		// abstract base class implemented by e.g. Model, Effect
		class Renderable {
		private:
			matrix4 transform;

		public:
			virtual ~Renderable() = 0;

			// issue draw command to renderer
			virtual void Draw(
				void
			) const = 0;
		};

	} // namespace Renderer

} // namespace XS
