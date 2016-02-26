#pragma once

#include <string>

namespace XS {

	// these are enumerated on load and tied to an artist/tool defined name
	typedef uint32_t QuestID;

#if defined(XS_QUEST_INTERNALS)
	class Quest {

	private:
		static QuestID numQuests;

	public:
		Quest();

		static Quest *GetFromString(
			std::string &str
		) XS_WARN_UNUSED_RESULT;

		static Quest *GetFromID(
			QuestID id
		) XS_WARN_UNUSED_RESULT;

	};
#endif

} // namespace XS
