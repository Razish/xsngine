#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSServer/XSAccount.h"

namespace XS {

	static std::vector<Account *> accounts;
	AccountID Account::numAccounts = 0u;

	// private
	Account::Account( const char *accountName )
	: privateID( numAccounts++ ), privateName( accountName ), privatePassword( "" ), privateLocked( false ),
		id( privateID ), name( privateName ), password( privatePassword ), locked( privateLocked )
	{
		console.Print( PrintLevel::Normal, "Creating account \"%s\" with ID %i\n",
			accountName,
			privateID
		);
		accounts.push_back( this );
	}

	// private
	Account::~Account() {
		const auto &it = std::find_if(
			std::begin( accounts ),
			std::end( accounts ),
			[this]( const Account *account ) {
				return account == this;
			}
		);
		if ( it != accounts.end() ) {
			accounts.erase( it );
		}
	}

	// public, static
	Account *Account::Find( AccountID accountID ) {
		const auto &it = std::find_if(
			std::begin( accounts ),
			std::end( accounts ),
			[accountID]( const Account *account ) {
				return account->id == accountID;
			}
		);

		if ( it == std::end( accounts ) ) {
			return nullptr;
		}
		return *it;
	}

	// public, static
	Account *Account::Find( const char *accountName ) {
		const auto &it = std::find_if(
			std::begin( accounts ),
			std::end( accounts ),
			[accountName]( const Account *account ) {
				return !String::CompareCase( account->name.c_str(), accountName );
			}
		);

		if ( it == std::end( accounts ) ) {
			return nullptr;
		}
		return *it;
	}

	// public, static
	Account *Account::Create( const char *accountName, const char *accountPassword ) {
		Account *account = new Account( accountName );
		account->privatePassword = accountPassword;
		return account;
	}

	bool Account::RequestLock( void ) {
		if ( privateLocked ) {
			return false;
		}
		privateLocked = true;
		return true;
	}

	void Account::ReleaseLock( void ) {
		privateLocked = false;
	}

} // namespace XS
