#pragma once

#include "XSCommon/XSCommand.h"

namespace XS {

	namespace Renderer {

		namespace Backend {

			// take a screenshot of the current display
			void Cmd_Screenshot(
				const CommandContext &context
			);

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
