#pragma once

#include "XSCommon/XSVector.h"

namespace XS {

	namespace Renderer {

		class Renderable;

		struct RenderInfo {
			uint32_t	 handle;
			vector3		 worldPos;
		};

		// base class implemented by e.g. Model, Particle
		class Renderable {
		public:
			static uint32_t			numRegistered;
			static const uint32_t	invalidHandle;

			uint32_t			handle;

			Renderable();
			~Renderable();

			static Renderable *Get(
				uint32_t handle
			);

			// register a renderable
			uint32_t Register(
				const char *path
			);

			// issue draw command to renderer
			virtual void Draw(
				const RenderInfo &info
			) const;

			// generic update e.g. for particle emitters
			void Update(
				real64_t dt
			);
		};

	} // namespace Renderer

} // namespace XS
