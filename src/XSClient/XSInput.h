#pragma once

namespace XS {

	namespace Client {

		// exists in XSClient/XSInput.cpp for now
		extern struct Input {
			Input();
			void Poll( void ) const;
		} input;

	} // namespace Client

} // namespace XS
