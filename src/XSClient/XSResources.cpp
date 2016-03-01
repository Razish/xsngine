#include <string>
#include <unordered_map>
#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSClient/XSResources.h"

namespace XS {

	namespace ClientGame {

		static std::unordered_map<ResourceID, std::string> resources;

		const char *GetResourceName( ResourceID id ) {
			return resources[id].c_str();
		}

	} // namespace ClientGame

} // namespace XS
