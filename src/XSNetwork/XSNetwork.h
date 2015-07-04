#pragma once

#include <RakNet/MessageIdentifiers.h>

namespace XS {

	namespace Network {

		enum GameMessage {
			// server --> client
			ID_XS_SV2CL_GAMESTATE = ID_USER_PACKET_ENUM + 1,
			ID_XS_SV2CL_REMOVE_ENTITY,
			ID_XS_SV2CL_PRINT,

			// client --> server
			ID_XS_CL2SV_GAMESTATE_ACK,
		};

		struct XSPacket {
			GameMessage		 msg;
			const void		*data = nullptr;
			size_t			 dataLen = 0u;

			// don't allow default instantiation
			XSPacket() = delete;
			XSPacket( const XSPacket& ) = delete;
			XSPacket& operator=( const XSPacket& ) = delete;

			XSPacket( GameMessage msg )
			: msg( msg )
			{
			}
		};

		// ???
		void Init(
			void
		);

		// ???
		void Shutdown(
			void
		);

		// ???
		bool IsConnected(
			void
		);

		// ???
		bool IsActive(
			void
		);

		// specifying 0 for the port will use the default server port
		bool Connect(
			const char *hostname,
			uint16_t port = 0u
		);

		// ???
		void Disconnect(
			void
		);

		// ???
		void Receive(
			void
		);

		// ???
		void Send(
			uint64_t guid,
			const XSPacket *packet
		);

		// ???
		void PrintStatus(
			void
		);

	} // namespace Network

} // namespace XS
