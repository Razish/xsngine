#pragma once

#include <vector>

#include "XSServer/XSCheckersBoard.h"

namespace XS {

	namespace ServerGame {

		extern struct GameState {
			CheckersPiece::Colour	currentMove;
		} state;

		// initialise the ServerGame
		void Init(
			void
		);

		void Shutdown(
			void
		);

		// run a frame
		void RunFrame(
			real64_t dt
		);

		// ???
		void UpdatePiece(
			uint8_t offsetFrom,
			uint8_t offsetTo
		);

		// ???
		void SetCurrentPlayer(
			CheckersPiece::Colour colour
		);

		// ???
		void Connect(
			const char *ip,
			uint64_t guid
		);

	} // namespace ServerGame

} // namespace XS
