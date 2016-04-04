#pragma once

#include <vector>

#include "XSServer/XSAccount.h"
#include "XSServer/XSInventoryManager.h"
#include "XSServer/XSQuestManager.h"

namespace XS {

	using CharacterID = uint32_t;

	// each Character is tied to an account on creation, but never belongs to a specific client or connection
	// each Account which has a list of characters that are tied to it
	// a Character is also used by a Client to interact with the world
	class Character {

	private:
		AccountID			privateAccountID;
		std::string			privateName;
		CharacterID			privateID;

		static CharacterID	numCharacters;

		Character(
			AccountID accountID,
			const char *name
		);
		~Character();

	public:
		// metadata
		AccountID			&accountID; // account ID of the owner
		const std::string	&name; // name of the character
		const CharacterID	&id; // character ID

		// game info
		InventoryManager	 inventory;
		QuestManager		 quests;

		// don't allow default instantiation
		Character() = delete;
		Character( const Character& ) = delete;
		Character& operator=( const Character& ) = delete;

		// find an existing character by name
		static Character *Find(
			AccountID accountID,
			const char *name
		);

		// list all characters owned by this account
		static void List(
			AccountID accountID,
			std::vector<Character *> &characterList
		);

		// create a new character for this account
		// will fail if this character name already exists
		static Character *Create(
			AccountID accountID,
			const char *name
		);

	};

} // namespace XS
