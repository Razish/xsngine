#pragma once

#include <RakNet/MessageIdentifiers.h>

namespace XS {

	namespace Network {

		enum GameMessage {
			ID_XS_PING = ID_USER_PACKET_ENUM + 1,
			ID_XS_TEXT_MESSAGE,
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

		// ???
		bool Connect(
			const char *hostname,
			uint16_t port
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
			const XSPacket *packet
		);

		// ???
		void PrintStatus(
			void
		);

	} // namespace Network

} // namespace XS
