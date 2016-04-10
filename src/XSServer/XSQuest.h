#pragma once

#include <vector>

namespace XS {

	// an QuestDefinition is a template for all derived QuestInstance objects
	class QuestDefinition {

	public:
		using ID = uint32_t;

	private:
		ID	privateID;

		static ID	numQuestDefinitions;

	public:
		const ID	&id;

		static const ID	invalidID;

		QuestDefinition();
		~QuestDefinition();

		static QuestDefinition *Find(
			ID definitionID
		);

	};

	class QuestInstance;

	// a QuestComponent stores a const reference to the QuestInstance it belongs to / will act upon
	// from there we can get to the QuestDefinition if we must
	class QuestComponent {
		friend class QuestInstance;

	public:
		using ID = uint32_t;

	private:
		ID	privateID;

		static ID	numQuestComponents;

	public:
		const ID			&id;
		const QuestInstance	&instance;

		static const ID	invalidID;

		// don't allow default instantiation
		QuestComponent() = delete;
		QuestComponent( const QuestComponent& ) = delete;
		QuestComponent& operator=( const QuestComponent& ) = delete;

		QuestComponent(
			QuestInstance &parentInstance
		);
		~QuestComponent();

		static QuestComponent *Find(
			ID componentID
		);

	};

	// each QuestInstance has a list of ItemComponent objects (inspired by the ECS concept) that are defined by the
	//	Item template
	class QuestInstance {
		friend class QuestComponent;

	public:
		using ID = uint32_t;

	private:
		ID	privateID;

		static ID	numQuestInstances;

	public:
		const ID						&id;
		const QuestDefinition			&definition;
		std::vector<QuestComponent *>	 components;

		static const ID	invalidID;

		QuestInstance(
			const QuestDefinition &questDefinition
		);
		~QuestInstance();

		static QuestInstance *Find(
			ID instanceID
		);

	};

} // namespace XS
