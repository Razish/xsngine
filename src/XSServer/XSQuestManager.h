#pragma once

#include "XSServer/XSQuest.h"

namespace XS {

	class QuestManager {

	private:
		std::vector<QuestID> activeQuests;
		QuestID currentQuest;

	public:
		// drop quest with the specific id
		bool Drop(
			QuestID id
		);

	};

} // namespace XS
