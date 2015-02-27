#pragma once

#include "XSCommon/XSVector.h"

namespace XS {

	class Cvar;

	namespace Client {

		namespace Input {

			extern Cvar *m_acceleration;
			extern Cvar *m_debug;
			extern Cvar *m_sensitivity;
			extern Cvar *m_smooth;
			extern Cvar *m_smoothFrames;

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
