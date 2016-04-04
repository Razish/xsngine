#pragma once

#include <unordered_map>

#include "XSNetwork/XSNetwork.h"
#include "XSServer/XSAccount.h"
#include "XSServer/XSCharacter.h"

namespace XS {

	namespace Server {

		class Client {

		private:
			// ...

		public:
			Network::Connection &connection;
			Account				*account;
			Character			*character;

			// don't allow default instantiation
			Client() = delete;
			Client( const Client& ) = delete;
			Client& operator=( const Client& ) = delete;

			// this also tracks it via clients[Client::connection::guid]
			Client( Network::Connection &connection );

			// this removes the [Client::connection::guid] association and invalidates iterators
			~Client();

			// print a message to the client's console
			// note: does not insert a line-feed at the end!
			void Print(
				const char *msg
			) const;

			// start using a new character
			// this will handle "logging out" of the current character etc
			void SelectCharacter(
				Character *newCharacter
			);

			// log into the specified account
			Account::LoginError Login(
				const char *name,
				const char *password
			);

			// log out of the current account
			void Logout(
				void
			);

		};

		extern std::unordered_map<Network::GUID, Client *> clients;

	} // namespace Server

} // namespace XS
