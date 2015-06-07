#pragma once

namespace XS {

	namespace Server {

		struct Client {
			enum class State {
				Connecting,
				Limbo,
				Playing
			};

			Client( uint64_t guid );

			// don't allow default instantiation
			Client() = delete;
			Client( const Client& ) = delete;
			Client& operator=( const Client& ) = delete;

			uint64_t	guid;
			State		state;
		};

	} // namespace ServerGame

} // namespace XS
