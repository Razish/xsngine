#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSClient/XSEntity.h"
#include "XSClient/XSClientGameState.h"
#include "XSRenderer/XSRenderable.h"
#include "XSRenderer/XSView.h"

namespace XS {

	namespace ClientGame {

		void AddEntity( Entity *entity ) {
			Entity *e = state.entities[entity->id];
			if ( e ) {
				SDL_assert( e != entity );
			}
			else {
				state.entities[entity->id] = entity;
			}
		}

		bool EntityExists( uint32_t id ) {
			return !!(state.entities[id] != nullptr);
		}

		Entity *GetEntity( uint32_t id ) {
			return state.entities[id];
		}

		void RemoveEntity( Entity *entity ) {
			state.entities.erase( entity->id );
		}

		Entity::~Entity() {
			if ( renderInfo.renderable ) {
				delete renderInfo.renderable;
				renderInfo.renderable = nullptr;
			}

			RemoveEntity( this );
		};

		void Entity::Update( real64_t dt ) {
			if ( renderInfo.renderable ) {
				renderInfo.renderable->Update( dt );
			}
		}

		void Entity::AddToScene( Renderer::View *view ) {
			renderInfo.worldPos = position;

			if ( renderInfo.renderable ) {
				view->AddObject( renderInfo );
			}
		}

	} // namespace ClientGame

} // namespace XS
