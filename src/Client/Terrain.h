#pragma once

class Cvar;

namespace Renderer {
	class ShaderProgram;
	class Texture;
	struct Material;
}

namespace ClientGame {

	class Entity;

	extern Cvar *cg_terrainPersistence;
	extern Cvar *cg_terrainFrequency;
	extern Cvar *cg_terrainAmplitude;
	extern Cvar *cg_terrainOctaves;
	extern Cvar *cg_terrainDimensions;

	class Terrain {

	private:
#if 0
		Renderer::ShaderProgram	*program = nullptr;
		Renderer::Material		*material = nullptr;
		Renderer::Texture		*dirtTexture = nullptr;
		Renderer::Texture		*grassTexture = nullptr;
		Renderer::Texture		*snowTexture = nullptr;
		Entity					*entity = nullptr;
#endif

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
