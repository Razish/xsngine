#pragma once

#include "XSCommon/XSVector.h"

namespace XS {

	class Cvar;

	namespace Client {

		extern Cvar *m_smooth;
		extern Cvar *m_smoothFrames;
		extern Cvar *m_sensitivity;
		extern Cvar *m_acceleration;

		// exists in XSClient/XSInput.cpp for now
		extern struct Input {
			Input();

			// check for new input events
			void Poll(
				void
			) const;

		} input;

		struct MovementCommand {
			vector3 viewAngles;
		};

		MovementCommand GenerateMovementCommand(
			real64_t dt
		);

	} // namespace Client

} // namespace XS
