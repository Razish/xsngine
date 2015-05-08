#pragma once

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSVector.h"

namespace XS {

	namespace Renderer {
		class Renderable;
	}

	namespace ClientGame {

		class GameObject {
		private:
			vector3 position;

		public:
			Renderer::Renderable *renderObject;

			GameObject()
			: renderObject( nullptr )
			{
			}
			virtual ~GameObject();

			const vector3 &GetPosition(
				void
			) const;

			void SetPosition(
				const vector3 &pos
			);

			virtual void Update(
				const real64_t dt
			);
		};

	} // namespace ClientGame

} // namespace XS
