#pragma once

#include <unordered_map>

#include "XSNetwork/XSNetwork.h"
#include "XSServer/XSCharacter.h"

namespace XS {

	namespace Server {

		class Client {

		private:
			// ...

		public:
			Network::Connection &connection;

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

			std::vector<Character>	 characters;
			Character				*currentCharacter;

		};

		extern std::unordered_map<Network::GUID, Client *> clients;

	} // namespace Server

} // namespace XS
