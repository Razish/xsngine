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
			if ( currentQuest == id ) {
				currentQuest = Quest::invalidID;
			}

			//TODO: network packet( removed quest X )

			return true;
		}

		SDL_assert( currentQuest != id && "not tracking the current/active quest?" );
		return false;
	}

} // namespace XS
