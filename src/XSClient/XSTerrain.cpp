#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSClient/XSClientGame.h"
#include "XSClient/XSEntity.h"
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

		Terrain::Terrain( const char *path ) {

			const size_t dimensions = cg_terrainDimensions->GetUInt32();

			//FIXME: seeds > 32k break the generator
			Perlin perlin(
				cg_terrainPersistence->GetReal64(), // persistence
				cg_terrainFrequency->GetReal64(), // frequency
				cg_terrainAmplitude->GetReal64(), // amplitude
				cg_terrainOctaves->GetInt32(), // octaves
				rand() & 0x7FFFu // seed
			);

			real32_t *heightmap = new real32_t[dimensions * dimensions];
			std::memset( heightmap, 0, dimensions * dimensions );
			for ( int row = 0; row < dimensions; row++ ) {
				for ( int col = 0; col < dimensions; col++ ) {
					real64_t p = perlin.GetHeight( row, col ); // [-1.0, 1.0]
					real64_t r = (p + 1.0) / 2.0; // [0.0, 1.0]
					heightmap[(row * dimensions) + col] = r;
				}
			}

			entity = new Entity();
			entity->renderInfo.renderable = Renderer::Model::Register( nullptr );
			Renderer::Mesh *mesh = new Renderer::Mesh();
			const real32_t scale = 64.0f;

			// positions
			mesh->vertices.reserve( dimensions * dimensions );
			for ( int row = 0; row < dimensions; row++ ) {
				const float fRow = static_cast<real32_t>( row );
				for ( int col = 0; col < dimensions; col++ ) {
					const float fCol = static_cast<real32_t>( col );
					const real32_t *sample = &heightmap[(row * dimensions) + col];
					vector3 pos = {
						fRow - (dimensions / 2.0f),
						(*sample * scale) - (scale / 1.5f),
						fCol - (dimensions / 2.0f)
					};
					mesh->vertices.push_back( pos );

					vector2 uv {
						fCol / dimensions,
						fRow / dimensions
					};
					mesh->UVs.push_back( uv );
				}
			}

			delete[] heightmap;

			// normals
			mesh->normals.reserve( dimensions * dimensions );
			for ( int row = 0; row < dimensions; row++ ) {
				for ( int col = 0; col < dimensions; col++ ) {
					//TODO: handle edge cases more gracefully
					vector3 normal;
					if ( row != dimensions - 1 && col != dimensions - 1 ) {
						vector3 a = mesh->vertices[row * dimensions + col];
						vector3 u = vector3( mesh->vertices[(row + 1) * dimensions + col] );
						vector3 v = vector3( mesh->vertices[(row + 1) * dimensions + (col + 1)] );
						normal = vector3::cross( u - a, v - a ) * -1.0f;
					}
					normal.normaliseFast();
					mesh->normals.push_back( normal );
				}
			}

			// indices
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

			dynamic_cast<Renderer::Model *>( entity->renderInfo.renderable )->meshes.push_back( mesh );
			AddEntity( entity );

			// create shader program
			static const Renderer::VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_Normal" },
				{ 2, "in_TexCoord" },
			};
			program = new Renderer::ShaderProgram( "terrain", "terrain", attributes, ARRAY_LEN( attributes ) );

			// create material
			material = new Renderer::Material();

			// create textures
			uint32_t outWidth = 0u, outHeight = 0u;

			// grass texture
			uint8_t *grassData = Renderer::LoadPNG( "textures/grass.png", &outWidth, &outHeight );
			grassTexture = new Renderer::Texture( outWidth, outHeight, Renderer::InternalFormat::RGBA8, grassData );
			delete grassData;
			Renderer::Material::SamplerBinding samplerBindingGrass;
			samplerBindingGrass.unit = 0;
			samplerBindingGrass.uniform = "u_grassTexture";
			samplerBindingGrass.texture = grassTexture;
			material->samplerBindings.push_back( samplerBindingGrass );

			// dirt texture
			uint8_t *dirtData = Renderer::LoadPNG( "textures/dirt.png", &outWidth, &outHeight );
			dirtTexture = new Renderer::Texture( outWidth, outHeight, Renderer::InternalFormat::RGBA8, dirtData );
			delete dirtData;
			Renderer::Material::SamplerBinding samplerBindingDirt;
			samplerBindingDirt.unit = 1;
			samplerBindingDirt.uniform = "u_dirtTexture";
			samplerBindingDirt.texture = dirtTexture;
			material->samplerBindings.push_back( samplerBindingDirt );

			// snow texture
			uint8_t *snowData = Renderer::LoadPNG( "textures/snow.png", &outWidth, &outHeight );
			snowTexture = new Renderer::Texture( outWidth, outHeight, Renderer::InternalFormat::RGBA8, snowData );
			delete snowData;
			Renderer::Material::SamplerBinding samplerBindingSnow;
			samplerBindingSnow.unit = 2;
			samplerBindingSnow.uniform = "u_snowTexture";
			samplerBindingSnow.texture = snowTexture;
			material->samplerBindings.push_back( samplerBindingSnow );

			real32_t averagePoint = 0.0f;
			real32_t highestPoint = 0.0f;
			for ( const auto &v : mesh->vertices ) {
				averagePoint += v[1];
				if ( v[1] > highestPoint ) {
					highestPoint = v[1];
				}
			}
			averagePoint /= static_cast<real32_t>( mesh->vertices.size() );
			program->SetUniform1( "u_lowPoint", averagePoint );
			program->SetUniform1( "u_highPoint", highestPoint );

			material->shaderProgram = program;
			//terrainMaterial->flags |= Renderer::MF_WIREFRAME;

			mesh->material = material;
		}

		Terrain::~Terrain() {
			RemoveEntity( entity );
			delete program;
			delete dirtTexture;
			delete grassTexture;
			delete snowTexture;
		}

	} // namespace ClientGame

} // namespace XS
