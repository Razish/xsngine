#pragma once

namespace XS {

	namespace Client {

		// exists in XSClient/XSInput.cpp for now
		extern struct Input {
			Input();

			// check for new input events
			void Poll(
				void
			) const;
		} input;

	} // namespace Client

} // namespace XS
