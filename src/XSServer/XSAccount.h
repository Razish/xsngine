#pragma once

#include <vector>

#include "XSServer/XSAccount.h"

namespace XS {

	using AccountID = uint32_t;

	//FIXME: this should probably all be moved out of the server/game and into an external service?
	//	or do we not care and just want it saved to host's disk?
	class Account {

	public:
		enum class LoginError : uint32_t {
			Success, // login succeeded
			AcctNotFound, // no account with this name
			BadAuthentication, // password did not match the account name
			Locked, // account is locked - logged in elsewhere
		};

	private:
		AccountID		privateID;
		std::string		privateName;
		std::string		privatePassword;
		bool			privateLocked;

		static AccountID	numAccounts;

		Account(
			const char *name
		);
		~Account();

	public:
		const AccountID		&id; // account ID
		const std::string	&name; // account name
		const std::string	&password; // account password
		const bool			&locked; // already logged in

		// don't allow default instantiation
		Account() = delete;
		Account( const Account& ) = delete;
		Account& operator=( const Account& ) = delete;

		// find an account by ID
		static Account *Find(
			AccountID accountID
		);

		// find an account by name
		static Account *Find(
			const char *name
		);

		// create a new account
		// will return nullptr if account already exists
		static Account *Create(
			const char *name,
			const char *password
		);

		// request a lock to this account - no one else can log into it
		bool RequestLock(
			void
		);

		// release the lock - allow others to log into it
		void ReleaseLock(
			void
		);

	};

} // namespace XS
