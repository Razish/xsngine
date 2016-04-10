#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSServer/XSInventoryManager.h"

namespace XS {

	static std::unordered_map<ItemDefinition::ID, ItemDefinition *> itemDefinitions;
	auto ItemDefinition::numItemDefinitions = 0u;
	const auto ItemDefinition::invalidID = std::numeric_limits<ItemDefinition::ID>::max();

	static std::unordered_map<ItemComponent::ID, ItemComponent *> itemComponents;
	auto ItemComponent::numItemComponents = 0u;
	const auto ItemComponent::invalidID = std::numeric_limits<ItemComponent::ID>::max();

	static std::unordered_map<ItemInstance::ID, ItemInstance *> itemInstances;
	auto ItemInstance::numItemInstances = 0u;
	const auto ItemInstance::invalidID = std::numeric_limits<ItemInstance::ID>::max();

	ItemDefinition::ItemDefinition()
	: privateID( numItemDefinitions++ ), id( privateID )
	{
		itemDefinitions[id] = this;
	}

	ItemDefinition::~ItemDefinition() {
		itemDefinitions[id] = nullptr;
	}

	// public, static
	ItemDefinition *ItemDefinition::Find( ItemDefinition::ID definitionID ) {
		const auto &it = itemDefinitions.find( definitionID );
		if ( it == std::end( itemDefinitions ) ) {
			return nullptr;
		}
		return it->second;
	}

	ItemComponent::ItemComponent( ItemInstance &parentInstance )
	: privateID( numItemComponents++ ), id( privateID ), instance( parentInstance )
	{
		itemComponents[id] = this;
	}

	ItemComponent::~ItemComponent() {
		itemComponents[id] = nullptr;
	}

	// public, static
	ItemComponent *ItemComponent::Find( ItemComponent::ID componentID ) {
		const auto &it = itemComponents.find( componentID );

		if ( it == std::end( itemComponents ) ) {
			return nullptr;
		}
		return it->second;
	}

	ItemInstance::ItemInstance( const ItemDefinition &itemDefinition )
	: privateID( numItemInstances++ ), id( privateID ), definition( itemDefinition )
	{
		itemInstances[id] = this;
	}

	ItemInstance::~ItemInstance() {
		itemInstances[id] = nullptr;
	}

	// public, static
	ItemInstance *ItemInstance::Find( ItemInstance::ID instanceID ) {
		const auto &it = itemInstances.find( instanceID );

		if ( it == std::end( itemInstances ) ) {
			return nullptr;
		}
		return it->second;
	}

} // namespace XS
