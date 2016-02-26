#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSServer/XSCharacter.h"

namespace XS {

	bool Character::DropQuest( QuestID id ) {
		return quests.Drop( id );
	}

} // namespace XS
