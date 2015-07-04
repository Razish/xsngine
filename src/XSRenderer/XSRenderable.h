#pragma once

#include "XSCommon/XSVector.h"

namespace XS {

	namespace Renderer {

		class Renderable;

		struct RenderInfo {
			Renderable	*renderable = nullptr;
			vector3		 worldPos;
		};

		// abstract base class implemented by e.g. Model, Particle
		class Renderable {
		public:
			virtual ~Renderable() = 0;

			// issue draw command to renderer
			virtual void Draw(
				const RenderInfo &info
			) const = 0;

			// generic update e.g. for particle emitters
			virtual void Update(
				real64_t dt
			);
		};

	} // namespace Renderer

} // namespace XS
