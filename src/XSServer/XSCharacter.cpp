#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSServer/XSCharacter.h"

namespace XS {

	static std::vector<Character *> characters;

	CharacterID Character::numCharacters = 0u;

	// private
	Character::Character( AccountID accountID, const char *name )
	: privateAccountID( accountID ), privateName( name ), privateID( numCharacters++ ),
		accountID( privateAccountID ), name( privateName ), id( privateID )
	{
		console.Print( PrintLevel::Normal, "Creating character \"%s\" with ID %i on account %i\n",
			privateName.c_str(),
			privateID,
			privateAccountID
		);
		characters.push_back( this );
	}

	// private
	Character::~Character() {
		const auto &it = std::find_if(
			std::begin( characters ),
			std::end( characters ),
			[this]( const Character *character ) {
				return character == this;
			}
		);
		if ( it != characters.end() ) {
			characters.erase( it );
		}
	}

	// public, static
	Character *Character::Find( AccountID accountID, const char *name ) {
		const auto &it = std::find_if(
			std::begin( characters ),
			std::end( characters ),
			[accountID, name]( const Character *character ) {
				return character->accountID == accountID && !String::Compare( character->name.c_str(), name );
			}
		);

		if ( it == std::end( characters ) ) {
			return nullptr;
		}
		return *it;
	}

	// public, static
	void Character::List( AccountID accountID, std::vector<Character *> &characterList ) {
		for ( Character *character : characters ) {
			if ( character->accountID == accountID ) {
				characterList.push_back( character );
			}
		}
	}

	// public, static
	Character *Character::Create( AccountID accountID, const char *name ) {
		Character *character = Find( accountID, name );
		if ( character ) {
			// character already exists
			return nullptr;
		}
		character = new Character( accountID, name );
		return character;
	}

} // namespace XS
