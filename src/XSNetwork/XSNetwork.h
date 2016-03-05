#pragma once

#include <RakNet/MessageIdentifiers.h>

namespace XS {

	namespace Network {

		using GUID = uint64_t;

		enum GameMessage {
			// server --> client
			ID_XS_SV2CL_GAMESTATE = ID_USER_PACKET_ENUM + 1,
			ID_XS_SV2CL_RESOURCES,
			ID_XS_SV2CL_PRINT,
			ID_XS_SV2CL_DUMMY,

			// client --> server
			ID_XS_CL2SV_COMMAND,
			ID_XS_CL2SV_DUMMY,
		};

		class XSPacket {

		private:
			// ...

		public:
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

			void Send(
				GUID guid
			) const;

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
		) XS_WARN_UNUSED_RESULT;

		// ???
		bool IsActive(
			void
		) XS_WARN_UNUSED_RESULT;

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
		void SendPacket(
			GUID guid,
			const XSPacket *packet
		);

		// ???
		void PrintStatus(
			void
		);

	} // namespace Network

} // namespace XS
