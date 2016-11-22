#pragma once

namespace ClientGame {

	using ResourceID = uint32_t;

	// ???
	const char *GetResourceName(
		ResourceID id
	) XS_WARN_UNUSED_RESULT;

} // namespace ClientGame
