#pragma once

#include "XSCommon/XSVector.h"

namespace XS {

	class Cvar;

	namespace Client {

		namespace Input {

			// initialise input state, register cvars, etc
			void Init(
				void
			);

			// poll for queued input events
			void Poll(
				void
			);

		} // namespace Input

	} // namespace Client

} // namespace XS
