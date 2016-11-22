#include <string>
#include <unordered_map>
#include <algorithm>

#include "Common/Common.h"
#include "Client/Resources.h"

namespace ClientGame {

	static std::unordered_map<ResourceID, std::string> resources;

	const char *GetResourceName( ResourceID id ) {
		return resources[id].c_str();
	}

} // namespace ClientGame
