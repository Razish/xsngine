#pragma once

#include "XSCommon/XSVector.h"

namespace Renderer {

	class Renderable;

	struct RenderInfo {

		uint32_t	 handle;
		vector3		 worldPos;

	};

	// base class implemented by e.g. Model, Particle
	class Renderable {

	public:
		using Handle = uint32_t;
		static Handle			numRegistered;
		static const Handle	invalidHandle;

		Handle			handle;

		Renderable();
		~Renderable();

		static Renderable *Get(
			Handle handle
		);

		// register a renderable
		Handle Register(
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
