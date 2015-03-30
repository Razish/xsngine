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

			GameObject();
			~GameObject();

			const vector3 &GetPosition(
				void
			) const;

			void SetPosition(
				const vector3 &pos
			);
		};

	} // namespace ClientGame

} // namespace XS
