#pragma once

#include <vector>

namespace XS {

	// an ItemDefinition is a template for all derived ItemInstance objects
	class ItemDefinition {

	public:
		using ID = uint32_t;

	private:
		ID	privateID;

		static ID	numItemDefinitions;

	public:
		const ID	&id;

		static const ID	invalidID;

		ItemDefinition();
		~ItemDefinition();

		static ItemDefinition *Find(
			ID definitionID
		);

	};

	class ItemInstance;

	// an ItemComponent stores a const reference to the ItemInstance it belongs to / will act upon
	// from there we can get to the ItemDefinition if we must
	class ItemComponent {
		friend class ItemInstance;

	public:
		using ID = uint32_t;

	private:
		ID	privateID;

		static ID	numItemComponents;

	public:
		const ID			&id;
		const ItemInstance	&instance;

		static const ID	invalidID;

		// don't allow default instantiation
		ItemComponent() = delete;
		ItemComponent( const ItemComponent& ) = delete;
		ItemComponent& operator=( const ItemComponent& ) = delete;

		ItemComponent(
			ItemInstance &parentInstance
		);
		~ItemComponent();

		static ItemComponent *Find(
			ID componentID
		);

	};

	// each ItemInstance has a list of ItemComponent objects (inspired by the ECS concept) that are defined by the
	//	Item template
	// an ItemInstance with no dynamic/modified components will instead have a "count" property and may be "stacked" or
	//	added to a list
	class ItemInstance {
		friend class ItemComponent;

	public:
		using ID = uint32_t;

	private:
		ID	privateID;

		static ID	numItemInstances;

	public:
		const ID						&id;
		const ItemDefinition			&definition;
		std::vector<ItemComponent *>	 components;

		static const ID	invalidID;

		ItemInstance(
			const ItemDefinition &itemDefinition
		);
		~ItemInstance();

		static ItemInstance *Find(
			ID instanceID
		);

	};

	class InventoryManager {

	private:
		// ...

	public:
		std::vector<ItemInstance *> items;

	};

} // namespace XS
