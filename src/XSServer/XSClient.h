#pragma once

#include <unordered_map>

namespace XS {

	namespace Server {

		struct Client;

		extern std::unordered_map<uint64_t, Client *>	clients;

		struct Client {
			enum class ConnectionState {
				Connecting,
				Limbo,
				Playing
			};

			uint64_t		guid;
			ConnectionState	connectionState;
		};

	} // namespace Server

} // namespace XS
