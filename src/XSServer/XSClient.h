#pragma once

namespace XS {

	namespace Server {

		struct Client {
			enum class ConnectionState {
				Connecting,
				Limbo,
				Playing
			};

			Client( uint64_t guid );

			// don't allow default instantiation
			Client() = delete;
			Client( const Client& ) = delete;
			Client& operator=( const Client& ) = delete;

			uint64_t		guid;
			ConnectionState	connectionState;
			uint64_t		lastSnapshotAcked = 0u;
		};

		Client *GetClient(
			uint64_t guid
		);

	} // namespace ServerGame

} // namespace XS
