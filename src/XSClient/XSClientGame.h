#pragma once

#include "XSCommon/XSVector.h"

namespace XS {

	class Cvar;

	namespace ClientGame {

		class GameObject;
		class FlyCamera;

		extern Cvar *cg_fov;
		extern FlyCamera *camera;

		extern struct GameState {
			vector3 viewAngles;
			vector3 viewDelta;
		} state;

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
			real64_t dt
		);

		// add an object to the game
		void AddObject(
			GameObject *obj
		);

		// remove an object from the game
		void RemoveObject(
			GameObject *obj
		);

		void MouseMotionEvent(
			const struct MouseMotionEvent &ev
		);

	} // namespace ClientGame

} // namespace XS
