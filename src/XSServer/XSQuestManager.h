#pragma once

#include "XSServer/XSQuest.h"

namespace XS {

	class QuestManager {

	private:
		// ...

	public:
		std::vector<QuestInstance::ID> activeQuests;
		QuestInstance::ID currentQuest = QuestInstance::invalidID;

		// drop quest with the specific id
		bool Drop(
			QuestInstance::ID id
		);

	};

} // namespace XS
