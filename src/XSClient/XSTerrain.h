#pragma once

namespace XS {

	class Cvar;

	namespace Renderer {
		class ShaderProgram;
		class Texture;
		struct Material;
	}

	namespace ClientGame {

		extern Cvar *cg_terrainPersistence;
		extern Cvar *cg_terrainFrequency;
		extern Cvar *cg_terrainAmplitude;
		extern Cvar *cg_terrainOctaves;
		extern Cvar *cg_terrainDimensions;

		class Terrain {
		private:
			Renderer::ShaderProgram	*program;
			Renderer::Material		*material;
			Renderer::Texture		*dirtTexture;
			Renderer::Texture		*grassTexture;
			Renderer::Texture		*snowTexture;
			GameObject				*gameObj;

		public:
			// don't allow default instantiation
			Terrain() = delete;
			Terrain( const Terrain& ) = delete;
			Terrain& operator=( const Terrain& ) = delete;

			// public constructor
			Terrain(
				const char *path
			);

			~Terrain();
		};

	} // namespace ClientGame

} // namespace XS
