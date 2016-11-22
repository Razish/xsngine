#pragma once

#include "Common/Command.h"

namespace Renderer {

	namespace Backend {

		// take a screenshot of the current display
		void Cmd_Screenshot(
			const CommandContext &context
		);

	} // namespace Backend

} // namespace Renderer
