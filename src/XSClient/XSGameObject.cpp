#include "XSClient/XSGameObject.h"
#include "XSRenderer/XSRenderable.h"

namespace XS {

	namespace ClientGame {

		GameObject::~GameObject() {
			if ( renderObject ) {
				delete renderObject;
				renderObject = nullptr;
			}
		};

		void GameObject::Update( real64_t dt ) {
			// ...
		}

		const vector3 &GameObject::GetPosition( void ) const {
			return position;
		}

		void GameObject::SetPosition( const vector3 &pos ) {
			position = pos;
		}

	} // namespace ClientGame

} // namespace XS
