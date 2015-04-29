#pragma once

namespace XS {

	class Cvar;

	namespace ClientGame {

		class GameObject;

		extern Cvar *cg_fov;

		// initialise the ClientGame, create a View and load all resources etc
		void Init(
			void
		);

		// run a frame, simulate entities
		void RunFrame(
			real64_t dt
		);

		// prepare the scene for rendering, adding renderable objects to the view
		void DrawFrame(
			void
		);

		// add an object to the game
		void AddObject(
			GameObject *obj
		);

		// remove an object from the game
		void RemoveObject(
			GameObject *obj
		);

	} // namespace ClientGame

} // namespace XS
