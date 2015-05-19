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
#include "XSRenderer/XSImagePNG.h"

namespace XS {

	namespace ClientGame {

		Cvar *cg_terrainPersistence = nullptr;
		Cvar *cg_terrainFrequency = nullptr;
		Cvar *cg_terrainAmplitude = nullptr;
		Cvar *cg_terrainOctaves = nullptr;
		Cvar *cg_terrainDimensions = nullptr;

		static uint8_t GetHeight( const uint8_t *data, size_t size, size_t row, size_t col ) {
			return data[(row * size) + col];
		}

		Terrain::Terrain( const char *path ) {

			const size_t dimensions = cg_terrainDimensions->GetUInt32();

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

			mesh->normals.reserve( dimensions * dimensions );
			for ( int row = 0; row < dimensions; row++ ) {
				for ( int col = 0; col < dimensions; col++ ) {
					//TODO: handle edge cases more gracefully
#if 0
					const real32_t x = GetHeight( heightmap, dimensions, col ? col - 1 : col, row )
						- GetHeight( heightmap, dimensions, col < (dimensions - 1) ? col + 1 : col, row );
					const real32_t z = GetHeight( heightmap, dimensions, col, row ? row - 1 : row )
						- GetHeight( heightmap, dimensions, col, row < (dimensions - 1) ? row + 1 : row );
#else
					const real32_t x = -(GetHeight( heightmap, dimensions, col + 1, row )
						- GetHeight( heightmap, dimensions, col - 1, row ));
					const real32_t z = GetHeight( heightmap, dimensions, col, row + 1 )
						- GetHeight( heightmap, dimensions, col, row - 1 );
#endif
					vector3 normal( x, 2.0f, z );
					normal.normaliseFast();
					mesh->normals.push_back( normal );
				}
			}

			delete[] heightmap;

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
			uint32_t outWidth = 0u, outHeight = 0u;
			uint8_t *dirtData = Renderer::LoadPNG( "textures/dirt.png", &outWidth, &outHeight );
			texture = new Renderer::Texture( outWidth, outHeight, Renderer::InternalFormat::RGBA8, dirtData );
			delete dirtData;

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
