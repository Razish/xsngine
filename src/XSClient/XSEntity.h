#pragma once

#include "XSCommon/XSVector.h"
#include "XSShared/XSEntityTypes.h"
#include "XSRenderer/XSRenderable.h"

namespace XS {

	namespace Renderer {
		struct View;
	}

	namespace ClientGame {

		class Entity {
		private:

		public:
			static const uint32_t	invalidID;

			uint32_t				id = invalidID;
			EntityType				type = EntityType::Generic;
			vector3					position;

			Renderer::RenderInfo	renderInfo;

			virtual ~Entity();

			virtual void Update(
				real64_t dt
			);

			void AddToScene(
				Renderer::View *view
			);
		};

		//TODO: move to EntityManager?

		// add an entity to the game
		void AddEntity(
			Entity *entity
		);

		// see if an entity with this ID already exists
		bool EntityExists(
			uint32_t id
		);

		// retrieve an entity by ID
		Entity *GetEntity(
			uint32_t id
		);

		// remove an entity from the game
		void RemoveEntity(
			Entity *entity
		);

	} // namespace ClientGame

} // namespace XS
