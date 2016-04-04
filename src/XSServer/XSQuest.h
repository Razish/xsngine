#pragma once

#include <string>

namespace XS {

	// these are enumerated on load and tied to an artist/tool defined name
	using QuestID = uint32_t;

	// a Quest does not contain an ID, but they are addressed by ID
	// a Quest serves as a template/factory for all QuestInstance objects to be generated from, using ideas inspired by
	//	the ECS concept
	class Quest {

	private:
		static QuestID numQuests;

	public:
		static const QuestID invalidID;

		Quest();

		static const Quest *GetFromString(
			std::string &str
		) XS_WARN_UNUSED_RESULT;

		static const Quest *GetFromID(
			QuestID id
		) XS_WARN_UNUSED_RESULT;

	};

} // namespace XS
