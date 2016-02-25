#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
	#define GLM_SWIZZLE
	#include <glm/glm.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <glm/ext.hpp>
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

		const uint32_t Entity::invalidID = 0xFFFFFFFFu;

		void AddEntity( Entity *entity ) {
			Entity *e = clgState.entities[entity->id];
			if ( e ) {
				SDL_assert( !"entity exists" );
			}
			else {
				clgState.entities[entity->id] = entity;
			}
		}

		bool EntityExists( uint32_t id ) {
			return !!(clgState.entities[id] != nullptr);
		}

		Entity *GetEntity( uint32_t id ) {
			return clgState.entities[id];
		}

		void RemoveEntity( uint32_t id ) {
			clgState.entities[id] = nullptr; // potential leak
		}

		Entity::~Entity() {
			RemoveEntity( id );

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

		void Entity::AddToScene( Renderer::View *view ) {
			if ( renderInfo.handle != Renderer::Renderable::invalidHandle ) {
				SDL_assert( view );

				renderInfo.worldPos = position;
				view->AddObject( renderInfo );
			}
		}

	} // namespace ClientGame

} // namespace XS
