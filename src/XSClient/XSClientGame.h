#pragma once

namespace XS {

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
			void
		);

	} // namespace ClientGame

} // namespace XS
