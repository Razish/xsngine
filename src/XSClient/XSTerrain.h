#pragma once

namespace XS {

	namespace Renderer {
		class ShaderProgram;
		class Texture;
		struct Material;
	}

	namespace ClientGame {

		class Terrain {
		private:
			Renderer::ShaderProgram	*program;
			Renderer::Material		*material;
			Renderer::Texture		*texture;

		public:
			// don't allow default instantiation
			Terrain() = delete;
			Terrain( const Terrain& ) = delete;
			Terrain& operator=( const Terrain& ) = delete;

			// public constructor
			Terrain(
				const char *path
			);
		};

	} // namespace ClientGame

} // namespace XS
