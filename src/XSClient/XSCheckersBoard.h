#pragma once

namespace XS {

	namespace Renderer {
		class ShaderProgram;
		class Texture;
		struct Material;
		class Font;
	} // namespace Renderer

	namespace ClientGame {

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

			void SendMovePacket(
				uint8_t toOffset
			);
		};

		// there is no internal storage of the checkers board
		// each piece maintains its 1D offset in the virtual board and which player it belongs to
		// this means the CheckersBoard is a miniscule 48 bytes!
		//	projected network throughput: ~200 bytes/sec at 2hz without delta compression
		//	48 bytes * 2hz + network overhead
		class CheckersBoard {
		private:
			static const uint8_t dimensions = 8u;
			static const uint8_t size = dimensions * dimensions;
			static const uint32_t numPlayers = 2u;
			static const uint8_t numPieces = (12u) * numPlayers;

			Renderer::ShaderProgram	*program = nullptr;
			Renderer::Material		*material = nullptr;
			Renderer::Texture		*boardTexture = nullptr;
			Renderer::Font			*font = nullptr;
			CheckersPiece			 pieces[numPieces]; // these are stored red then black
			CheckersPiece			*selectedPiece = nullptr;
			std::vector<uint8_t>	 possibleMoves;

			void CalculatePossibleMoves(
				void
			);

		public:
			CheckersBoard();
			~CheckersBoard();

			void Update(
				real64_t dt
			);

			void Render(
				real64_t dt
			) const;

			void MouseButtonEvent(
				real32_t cursorX,
				real32_t cursorY
			);

			// triggered by network event
			void UpdatePiece(
				uint8_t offsetFrom,
				uint8_t offsetTo
			);
		};

	} // namespace ClientGame

} // namespace XS
