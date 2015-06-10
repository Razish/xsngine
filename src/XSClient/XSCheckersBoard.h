#pragma once

namespace XS {

	namespace Renderer {
		class ShaderProgram;
		class Texture;
		struct Material;
		class Font;
	} // namespace Renderer

	namespace ClientGame {

		// there is no internal storage of the checkers board
		// each piece maintains its 1D offset in the virtual board

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

			void Move(
				uint8_t x,
				uint8_t y
			);
		};

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

		public:
			CheckersBoard();
			~CheckersBoard();

			void Update(
				real64_t dt
			);

			void Render(
				real64_t dt
			);
		};

	} // namespace ClientGame

} // namespace XS
