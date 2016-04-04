#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>
#include <limits>

#include "XSCommon/XSCommon.h"
#include "XSServer/XSQuest.h"

namespace XS {

	static std::vector<Quest *> quests; // indexed by QuestIDs
	static std::unordered_map<std::string, QuestID> questIDs;
	QuestID Quest::numQuests = 0u;
	const QuestID Quest::invalidID = std::numeric_limits<QuestID>::max();

	Quest::Quest() {
		// ...
	}

	// public, static
	const Quest *Quest::GetFromString( std::string &str ) {
		auto it = questIDs.find( str );
		if ( it == questIDs.end() ) {
			// lazy initialise
			Quest *quest = new Quest();
			quests.push_back( quest );
			return quest;
		}
		return GetFromID( it->second );
	}

	// public, static
	const Quest *Quest::GetFromID( QuestID id ) {
		return quests[id];
	}

} // namespace XS
