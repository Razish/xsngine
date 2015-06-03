#pragma once

namespace XS {

	namespace Renderer {
		class Renderable;
		struct View;
	}

	namespace ClientGame {

		class Entity {
		private:

		public:
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
