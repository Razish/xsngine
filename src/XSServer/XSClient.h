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
			class Connection {

			private:
				// ...

			public:
				Network::GUID guid;

				// send a packet to this client
				void Send(
					const Network::XSPacket *packet
				);

			} connection;

			enum class State {
				Connecting,
				Limbo,
				Playing
			} state;

			std::vector<Character>	 characters;
			Character				*currentCharacter;

		};

		extern std::unordered_map<Network::GUID, Client *> clients;

	} // namespace Server

} // namespace XS
