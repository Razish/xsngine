#pragma once

#include <vector>

#include "XSServer/XSInventoryManager.h"
#include "XSServer/XSQuestManager.h"

namespace XS {

	class Character {
	private:
		// inventory
		InventoryManager inventory;

		// quest data
		QuestManager quests;

	public:
		bool DropQuest(
			QuestID id
		) XS_WARN_UNUSED_RESULT;

	};

} // namespace XS
