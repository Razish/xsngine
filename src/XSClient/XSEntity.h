#pragma once

#include "XSCommon/XSVector.h"

namespace XS {

	namespace Renderer {
		class Renderable;
		struct View;
	}

	namespace ClientGame {

		class Entity {
		private:

		public:
			uint32_t				 id;
			vector3					 position;
			Renderer::Renderable	*renderObject = nullptr;

			virtual ~Entity();

			virtual void Update(
				const real64_t dt
			);

			void AddToScene(
				Renderer::View *view
			) const;
		};

	} // namespace ClientGame

} // namespace XS
