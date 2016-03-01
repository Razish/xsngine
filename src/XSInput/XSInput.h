#pragma once

#include "XSCommon/XSVector.h"

namespace XS {

	class Cvar;

	namespace Client {

		namespace Input {

			extern Cvar *m_sensitivity;

			// per frame, accumulates until a movement command is generated
			extern struct PerFrameState {

				bool	moveForward, moveBack;
				bool	moveLeft, moveRight;
				bool	moveUp, moveDown;

			} perFrameState;

			//TODO: move movement command generation to gamecode?
			struct MovementCommand {

				vector3 viewAngles;
				uint8_t buttonState;
				struct {

					int32_t FB : 2; // -1 for back, 0 for neutral, 1 for forward
					int32_t RL : 2; // -1 for left, 0 for neutral, 1 for right
					int32_t UD : 2; // -1 for down, 0 for neutral, 1 for up

				} move;
				//TODO: add a way to express analogue input for controllers
				//	can we abuse directions to compress the vector?

			};

			// initialise input state, register cvars, etc
			void Init(
				void
			);

			// poll for queued input events
			void Poll(
				void
			);

			// allocate a movement command and commit to history
			MovementCommand &AllocateMovementCommand(
				void
			) XS_WARN_UNUSED_RESULT;

			// generate a movement command for this frame's accumulated input
			void GenerateMovementCommand(
				MovementCommand &cmd
			);

		} // namespace Input

	} // namespace Client

} // namespace XS
