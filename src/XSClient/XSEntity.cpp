#include "XSCommon/XSCommon.h"
#include "XSClient/XSEntity.h"
#include "XSRenderer/XSRenderable.h"
#include "XSRenderer/XSView.h"

namespace XS {

	namespace ClientGame {

		Entity::~Entity() {
			if ( renderObject ) {
				delete renderObject;
				renderObject = nullptr;
			}
		};

		void Entity::Update( real64_t dt ) {
			// ...
		}

		void Entity::AddToScene( Renderer::View *view ) const {
			view->AddObject( renderObject );
		}

	} // namespace ClientGame

} // namespace XS
