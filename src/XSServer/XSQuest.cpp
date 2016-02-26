#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>

#include "XSCommon/XSCommon.h"

#define XS_QUEST_INTERNALS
#include "XSServer/XSQuest.h"
#undef XS_QUEST_INTERNALS

namespace XS {

	static std::vector<Quest *> quests;
	static std::unordered_map<std::string, QuestID> questIDs;
	QuestID Quest::numQuests = 0u;

	Quest *Quest::GetFromString( std::string &str ) {
		auto it = questIDs.find( str );
		if ( it == questIDs.end() ) {
			// lazy initialise
			Quest *quest = new Quest();
			quests.push_back( quest );
			return quest;
		}
		return GetFromID( it->second );
	}

	Quest *Quest::GetFromID( QuestID id ) {
		return quests[id];
	}

} // namespace XS
