#include <algorithm>
#include <vector>

#include "XSCommon/XSCommon.h"
#include "XSServer/XSQuestManager.h"

namespace XS {

	bool QuestManager::Drop( QuestID id ) {
		auto it = std::find( activeQuests.begin(), activeQuests.end(), id );
		if ( it != activeQuests.end() ) {
			// found it, remove it
			activeQuests.erase( it );

			//TODO: network packet( removed quest X )

			return true;
		}

		return false;
	}

} // namespace XS
