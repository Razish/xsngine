#pragma once

#include <vector>

#include "XSCommon/XSVector.h"
#include "XSClient/XSCheckersBoard.h"

namespace XS {

	namespace ClientGame {

		extern struct GameState {
			vector2					cursorPos;
			bool					playing;
			CheckersPiece::Colour	currentPlayer;
		} state;

	} // namespace ClientGame

} // namespace XS
