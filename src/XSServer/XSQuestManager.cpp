#include <algorithm>
#include <vector>

#include "XSCommon/XSCommon.h"
#include "XSServer/XSQuestManager.h"

namespace XS {

	bool QuestManager::Drop( QuestInstance::ID id ) {
		auto it = std::find( activeQuests.begin(), activeQuests.end(), id );
		if ( it != activeQuests.end() ) {
			// found it, remove it
			if ( currentQuest == id ) {
				currentQuest = QuestInstance::invalidID;
			}

			//TODO: network packet( removed quest X )

			//TODO: clean up quest data

			activeQuests.erase( it );
			return true;
		}

		SDL_assert( currentQuest != id && "not tracking the current/active quest?" );
		return false;
	}

} // namespace XS
