#pragma once

#include "Common/Vector.h"
#include "Shared/EntityTypes.h"
#include "Renderer/Renderable.h"
#include "Renderer/View.h"

namespace ClientGame {

	// you must call Entity::AddToWorld to perform logic on it and render it
	// you must also call Entity::AddToScene in order to render the entity. this will fill in the
	//	Renderer::RenderInfo struct and pass it off to the renderer
	class Entity {

	public:
		using ID = uint32_t;

	private:
		// ...

	public:
		static const ID		invalidID;

		ID			id = invalidID;
		EntityType	type = EntityType::Generic;
		vector3		position;

		Renderer::RenderInfo	renderInfo;

		virtual ~Entity();

		virtual void Update(
			real64_t dt
		);

		// add an entity to the game
		void AddToWorld(
			void
		);

		// see if an entity with this ID already exists
		static bool Exists(
			ID id
		) XS_WARN_UNUSED_RESULT;

		// retrieve an entity by ID
		static Entity *Get(
			ID id
		) XS_WARN_UNUSED_RESULT;

		// render an entity to the currently bound View
		void AddToScene(
			Renderer::View &view
		);

	};

} // namespace ClientGame
