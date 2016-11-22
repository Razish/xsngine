#pragma once

#include <limits>
#include <RakNet/MessageIdentifiers.h>

namespace Network {

	using GUID = uint64_t;

	enum GameMessage {
		//
		// server --> client
		//

		// offset	size	description
		// ------	----	-----------
		// 00		0		N/A
		ID_XS_SV2CL_DUMMY = ID_USER_PACKET_ENUM + 1,

		// let the client know what their new connection state should be
		// offset	size	description
		// ------	----	-----------
		// 00		4		unsigned 32 bit integer representing new connection state
		ID_XS_SV2CL_CONNECTION_STATE,

		// offset	size	description
		// ------	----	-----------
		// 00		????	????
		ID_XS_SV2CL_GAMESTATE,

		// offset	size	description
		// ------	----	-----------
		// 00		????	????
		ID_XS_SV2CL_RESOURCES,

		// offset	size	description
		// ------	----	-----------
		// 00		s		string to print to console (line-feed is not appended)
		ID_XS_SV2CL_PRINT,

		// offset	size	description
		// ------	----	-----------
		// 00		64		unsigned 64 bit integer: identifier of the sender
		// 64		4		unsigned 32 bit integer: reserved for message flags (e.g. private channel info)
		// 68		s		string of the chat message
		ID_XS_SV2CL_CHAT,

		//
		// client --> server
		//

		// let the server know what our new connection state should be
		// offset	size	description
		// ------	----	-----------
		// 00		4		unsigned 32 bit integer representing new connection state
		ID_XS_CL2SV_CONNECTION_STATE,

		// offset	size	description
		// ------	----	-----------
		// 00		4		numArgs
		// 04		n*s		list of strings
		ID_XS_CL2SV_COMMAND,

		// 00		0		N/A
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

	class Connection {

	public:
		// stateful connection between hosts
		//	state					action										direction of communication
		//	-----					------										--------------------------
		//	NotConnected			connect to server							client     server
		//	-> SynSent				request connection							client  -> server
		//	-> SynReceived			generate challenge for client				client <-  server
		//	-> AckSent				respond to challenge						client  -> server
		//	-> AckReceived			challenge accepted, sending session info	client <-  server
		//	-> Active				baselines have been created					client  -> server
		//		-> Disconnecting	disconnection has been requested			client <-> server
		//			-> Dead			host disconnected or timed out				client     server
		//		-> Dead				host disconnected or timed out				client     server
		//
		// dev note: if you add/remove any states, also modify StateToString
		// dev note: comments are from the perspective of a client connecting to a server
		enum class State : uint32_t {
			NotConnected = 0,	// initial state: no connections requested
			SynSent,			// we request a connection to a host
			SynReceived,		// the host sends us back a challenge
			AckSent,			// we respond to the challenge
			AckReceived,		// host allocates client instance and starts sending session info
			Active,				// baselines have been receieved, connection is active
			Disconnecting,		// disconnection has been requested
			Dead,				// this host has finished disconnecting or has timed out
		};

	private:
		State privateState = State::NotConnected;

	public:
		const State &state;
		GUID guid;

		// don't allow default instantiation
		Connection() = delete;
		Connection& operator=( const Connection& ) = delete;

		Connection( GUID guid );
		~Connection();

		// get a Connection from a GUID
		static Connection &Get(
			GUID guid
		);

		// string representation of each connection state
		static const char *StateToString(
			State state
		);

		// send a packet to this Connection
		void Send(
			const XSPacket &packet
		) const;

		// change the connection state and send a notifying packet
		// returns true if the change was valid
		bool ChangeState(
			State newState,
			bool doNetwork = false
		);

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
		const XSPacket &packet
	);

	// ???
	void PrintStatus(
		void
	);

} // namespace Network
