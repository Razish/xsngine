#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
	#pragma GCC diagnostic pop
#endif

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSClient/XSEntity.h"
#include "XSClient/XSClientGameState.h"
#include "XSRenderer/XSRenderable.h"
#include "XSRenderer/XSView.h"

namespace XS {

	namespace ClientGame {

		const Entity::ID Entity::invalidID = 0xFFFFFFFFu;

		void Entity::AddToWorld( void ) {
			Entity *e = clgState.entities[id];
			if ( e ) {
				if ( e == this ) {
					SDL_assert( !"tried to add entity to world twice" );
				}
				else {
					SDL_assert( !"entity with this id already exists" );
				}
			}
			else {
				clgState.entities[id] = this;
			}
		}

		Entity::~Entity() {
			if ( clgState.entities[id] == this ) {
				clgState.entities[id] = nullptr; // potential leak
			}
			else {
				SDL_assert( !"tried to untrack entity with mismatched ID" );
			}

			//FIXME: this isn't right...?
			if ( renderInfo.handle != Renderer::Renderable::invalidHandle ) {
				//Renderer::Model::Free( renderInfo.handle );
				renderInfo.handle = Renderer::Renderable::invalidHandle;
			}
		};

		void Entity::Update( real64_t dt ) {
			if ( renderInfo.handle != Renderer::Renderable::invalidHandle ) {
				Renderer::Renderable::Get( renderInfo.handle )->Update( dt );
			}
		}

		bool Entity::Exists( ID id ) {
			return !!(clgState.entities[id] != nullptr);
		}

		Entity *Entity::Get( ID id ) {
			return clgState.entities[id];
		}

		void Entity::AddToScene( Renderer::View *view ) {
			SDL_assert( view );

			if ( renderInfo.handle != Renderer::Renderable::invalidHandle ) {
				renderInfo.worldPos = position;
				view->AddObject( renderInfo );
			}
		}

	} // namespace ClientGame

} // namespace XS
