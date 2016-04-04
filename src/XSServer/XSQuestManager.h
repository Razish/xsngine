#pragma once

#include "XSServer/XSQuest.h"

namespace XS {

	class QuestManager {

	private:
		// ...

	public:
		std::vector<QuestID> activeQuests;
		QuestID currentQuest = Quest::invalidID;

		// drop quest with the specific id
		bool Drop(
			QuestID id
		);

	};

} // namespace XS
