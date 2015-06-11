#pragma once

namespace XS {

	class Cvar;

	namespace ClientGame {

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

		void MouseMotionEvent(
			const struct MouseMotionEvent &ev
		);

		void MouseButtonEvent(
			const struct MouseButtonEvent &ev
		);

	} // namespace ClientGame

} // namespace XS
