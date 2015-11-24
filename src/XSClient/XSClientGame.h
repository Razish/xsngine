#pragma once

namespace RakNet {
	struct Packet;
} // namespace RakNet

namespace XS {

	class Cvar;

	namespace ClientGame {

		class Entity;
		class FlyCamera;

		extern Cvar *cg_fov;
		extern FlyCamera *camera;

		// initialise the ClientGame, create a View and load all resources etc
		void Init(
			void
		);

		// ???
		void Shutdown(
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

		// received a network packet
		bool ReceivePacket(
			const RakNet::Packet *packet
		);

		// ???
		void MouseMotionEvent(
			const struct MouseMotionEvent &ev
		);

		// ???
		void MouseButtonEvent(
			const struct MouseButtonEvent &ev
		);

	} // namespace ClientGame

} // namespace XS
