#pragma once

namespace XS {

	namespace ServerGame {

		class CheckersPiece {
		public:
			enum class Colour : uint8_t {
				Red,
				Black,
			};

			// offset from the virtual board
			// for a given x,y let offset = (x * rowSize) + y
			uint8_t		offset;
			Colour		colour;
			bool		valid = true;
			bool		king = false;
		};

		// there is no internal storage of the checkers board
		// each piece maintains its 1D offset in the virtual board and which player it belongs to
		class CheckersBoard {
		private:
			static const uint8_t dimensions = 8u;
			static const uint8_t size = dimensions * dimensions;
			static const uint32_t numPlayers = 2u;
			static const uint8_t numPieces = (12u) * numPlayers;

			CheckersPiece			 pieces[numPieces]; // these are stored red then black

		public:
			CheckersBoard();

			void Update(
				real64_t dt
			);

			// triggered by network event
			void UpdatePiece(
				uint8_t offsetFrom,
				uint8_t offsetTo
			);
		};

	} // namespace ServerGame

} // namespace XS
