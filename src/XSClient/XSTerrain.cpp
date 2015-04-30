#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSClient/XSClientGame.h"
#include "XSClient/XSGameObject.h"
#include "XSClient/XSTerrain.h"
#include "XSClient/XSPerlin.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSModel.h"
#include "XSRenderer/XSMesh.h"
#include "XSRenderer/XSVertexAttributes.h"

//#define FILE_TERRAIN

namespace XS {

	namespace ClientGame {

		Cvar *cg_terrainPersistence = nullptr;
		Cvar *cg_terrainFrequency = nullptr;
		Cvar *cg_terrainAmplitude = nullptr;
		Cvar *cg_terrainOctaves = nullptr;
		Cvar *cg_terrainDimensions = nullptr;

		Terrain::Terrain( const char *path ) {
#ifdef FILE_TERRAIN
			const File f( path );
			if ( !f.open ) {
				console.Print( PrintLevel::Normal, "WARNING: Could not load terrain heightmap \"%s\"\n", f.path );
				return;
			}
#endif

#ifdef FILE_TERRAIN
			const size_t dimensions = sqrt( f.length );
#else
			const size_t dimensions = cg_terrainDimensions->GetUInt32();
#endif

#ifdef FILE_TERRAIN
			uint8_t *heightmap = new uint8_t[f.length];
			std::memset( heightmap, 0, f.length );
			f.Read( heightmap );
#else
			Perlin perlin(
				cg_terrainPersistence->GetReal64(), // persistence
				cg_terrainFrequency->GetReal64(), // frequency
				cg_terrainAmplitude->GetReal64(), // amplitude
				cg_terrainOctaves->GetInt32(), // octaves
				rand() // seed
			);

			uint8_t *heightmap = new uint8_t[dimensions * dimensions];
			std::memset( heightmap, 0, dimensions * dimensions );
			for ( int row = 0; row < dimensions; row++ ) {
				for ( int col = 0; col < dimensions; col++ ) {
					real64_t p = perlin.GetHeight( row, col ); // [-1.0, 1.0]
					real64_t r = (p + 1.0) / 2.0; // [0.0, 1.0]
					heightmap[(row * dimensions) + col] = static_cast<uint8_t>( r * 128.0 );
				}
			}
#endif

			gameObj = new GameObject();
			gameObj->renderObject = Renderer::Model::Register( nullptr );
			Renderer::Mesh *mesh = new Renderer::Mesh();
			const real32_t scale = 128.0f;

			mesh->vertices.reserve( dimensions * dimensions );
			for ( int row = 0; row < dimensions; row++ ) {
				const float fRow = static_cast<real32_t>( row );
				for ( int col = 0; col < dimensions; col++ ) {
					const float fCol = static_cast<real32_t>( col );
					const uint8_t *sample = &heightmap[(row * dimensions) + col];
					vector3 pos;
					pos.x = -fRow;
					pos.y = (static_cast<real32_t>( *sample ) / 255.0f) * scale;
					pos.z = -fCol;
					mesh->vertices.push_back( pos );

					vector2 uv;
					uv.x = fCol / dimensions;
					uv.y = fRow / dimensions;
					mesh->UVs.push_back( uv );
				}
			}

			mesh->indices.reserve( (dimensions - 1) * (dimensions - 1) * 6 );
			for ( int row = 0; row < dimensions - 1; row++ ) {
				for ( int col = 0; col < dimensions - 1; col++ ) {
					const int32_t start = (row * dimensions) + col;
					mesh->indices.push_back( start );
					mesh->indices.push_back( start + 1 );
					mesh->indices.push_back( start + dimensions );

					mesh->indices.push_back( start + 1 );
					mesh->indices.push_back( start + 1 + dimensions );
					mesh->indices.push_back( start + dimensions );
				}
			}

			mesh->Upload();

			dynamic_cast<Renderer::Model *>( gameObj->renderObject )->meshes.push_back( mesh );
			AddObject( gameObj );

			// create texture
			texture = new Renderer::Texture( dimensions, dimensions, Renderer::InternalFormat::R8, heightmap );

			delete[] heightmap;

			// create shader program
			static const Renderer::VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_Normal" },
				{ 2, "in_TexCoord" },
			};
			program = new Renderer::ShaderProgram( "model", "model", attributes, ARRAY_LEN( attributes ) );

			// create material
			material = new Renderer::Material();
			Renderer::Material::SamplerBinding samplerBinding;
			samplerBinding.unit = 0;
			samplerBinding.texture = texture;
			material->samplerBindings.push_back( samplerBinding );
			material->shaderProgram = program;
			//terrainMaterial->flags |= Renderer::MF_WIREFRAME;

			mesh->material = material;
		}

		Terrain::~Terrain() {
			RemoveObject( gameObj );
			delete gameObj;
			delete program;
			delete texture;
		}

	} // namespace ClientGame

} // namespace XS
