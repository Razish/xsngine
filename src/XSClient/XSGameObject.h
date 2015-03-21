#pragma once

#include "XSRenderer/XSRenderable.h"

namespace XS {

	namespace ClientGame {

		class GameObject {
		private:
			// ...

		public:
			Renderer::Renderable *renderObject;

			GameObject()
			: renderObject( nullptr )
			{
			};

			~GameObject() {
				if ( renderObject ) {
					delete renderObject;
					renderObject = nullptr;
				}
			};
		};

	} // namespace ClientGame

} // namespace XS
