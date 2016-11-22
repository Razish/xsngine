#pragma once

#include <vector>

#include "Common/Timer.h"
#include "Network/Network.h"

namespace RakNet {
	struct Packet;
} // namespace RakNet

class Cvar;

namespace Server {

	extern Cvar		*sv_maxConnections;
	extern uint64_t	 frameNum;

	// initialise the Server object, loading related subsystems
	void Init(
		void
	);

	// shut down the server object
	// only called on application quit
	void Shutdown(
		void
	);

	// pump any network events for the ServerGame
	void NetworkPump(
		void
	);

	// a network pump receieved a packet intended for the Server to receive
	bool ReceivePacket(
		const RakNet::Packet *packet
	);

	// ???
	void IncomingConnection(
		Network::Connection &connection
	);

	// ???
	void DropClient(
		Network::GUID guid
	);

	// run a frame and pass control to the ServerGame
	void RunFrame(
		real64_t dt
	);

	// broadcast a message to all players
	void BroadcastMessage(
		const char *msg
	);

	// get the current elapsed time
	// lazily initialised on each unique frame
	real64_t GetElapsedTime(
		TimerResolution resolution = TimerResolution::Milliseconds
	) XS_WARN_UNUSED_RESULT;

} // namespace Server
