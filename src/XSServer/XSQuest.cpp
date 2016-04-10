#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSServer/XSQuest.h"

namespace XS {

	static std::unordered_map<QuestDefinition::ID, QuestDefinition *> questDefinitions;
	auto QuestDefinition::numQuestDefinitions = 0u;
	const auto QuestDefinition::invalidID = std::numeric_limits<QuestDefinition::ID>::max();

	static std::unordered_map<QuestDefinition::ID, QuestComponent *> questComponents;
	auto QuestComponent::numQuestComponents = 0u;
	const auto QuestComponent::invalidID = std::numeric_limits<QuestComponent::ID>::max();

	static std::unordered_map<QuestDefinition::ID, QuestInstance *> questInstances;
	auto QuestInstance::numQuestInstances = 0u;
	const auto QuestInstance::invalidID = std::numeric_limits<QuestInstance::ID>::max();

	QuestDefinition::QuestDefinition()
	: privateID( numQuestDefinitions++ ), id( privateID )
	{
		questDefinitions[id] = this;
	}

	QuestDefinition::~QuestDefinition() {
		questDefinitions[id] = nullptr;
	}

	// public, static
	QuestDefinition *QuestDefinition::Find( QuestDefinition::ID definitionID ) {
		const auto &it = questDefinitions.find( definitionID );
		if ( it == std::end( questDefinitions ) ) {
			return nullptr;
		}
		return it->second;
	}

	QuestComponent::QuestComponent( QuestInstance &parentInstance )
	: privateID( numQuestComponents++ ), id( privateID ), instance( parentInstance )
	{
		questComponents[id] = this;
	}

	QuestComponent::~QuestComponent() {
		questComponents[id] = nullptr;
	}

	// public, static
	QuestComponent *QuestComponent::Find( QuestComponent::ID componentID ) {
		const auto &it = questComponents.find( componentID );

		if ( it == std::end( questComponents ) ) {
			return nullptr;
		}
		return it->second;
	}

	QuestInstance::QuestInstance( const QuestDefinition &questDefinition )
	: privateID( numQuestInstances++ ), id( privateID ), definition( questDefinition )
	{
		questInstances[id] = this;
	}

	QuestInstance::~QuestInstance() {
		questInstances[id] = nullptr;
	}

	// public, static
	QuestInstance *QuestInstance::Find( QuestInstance::ID instanceID ) {
		const auto &it = questInstances.find( instanceID );

		if ( it == std::end( questInstances ) ) {
			return nullptr;
		}
		return it->second;
	}

} // namespace XS
