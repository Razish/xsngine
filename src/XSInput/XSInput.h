#pragma once

#include "XSCommon/XSVector.h"

namespace XS {

	class Cvar;

	namespace Client {

		namespace Input {

			extern Cvar *debug_mouse;
			extern Cvar *m_acceleration;
			extern Cvar *m_sensitivity;
			extern Cvar *m_smooth;
			extern Cvar *m_smoothFrames;

			// per frame, accumulates until a movement command is generated
			extern struct PerFrameState {
				pvector2 mouseDelta;
				bool moveForward;
				bool moveBack;
				bool moveLeft;
				bool moveRight;
				bool moveUp;
				bool moveDown;
			} perFrameState;

			//TODO: move movement command generation to gamecode?
			struct MovementCommand {
				vector3 viewAngles;
				uint8_t buttonState;
			};

			// initialise input state, register cvars, etc
			void Init(
				void
			);

			// poll for queued input events
			void Poll(
				void
			);

			// generate a movement command for this frame's accumulated input
			MovementCommand GenerateMovementCommand(
				real64_t dt
			);

		} // namespace Input

	} // namespace Client

} // namespace XS
