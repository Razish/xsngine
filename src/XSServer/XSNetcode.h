#pragma once

#include <unordered_map>

#include "XSServer/XSServerGame.h"

class ByteBuffer;

namespace ServerGame {

	struct Baseline {

		GameState	state;

	};

	extern struct NetworkState {

		std::unordered_map<Network::GUID, Baseline *>	baseLines; // per Client, key is client's GUID

	} net;

	// ???
	void GenerateSnapshot(
		ByteBuffer *buffer
	);

} // namespace ServerGame
