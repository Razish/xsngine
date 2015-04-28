#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSConsole.h"
#include "XSClient/XSClientGame.h"
#include "XSClient/XSGameObject.h"
#include "XSClient/XSTerrain.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSModel.h"
#include "XSRenderer/XSMesh.h"
#include "XSRenderer/XSVertexAttributes.h"

#define FILE_TERRAIN

namespace XS {

	namespace ClientGame {

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
			const size_t dimensions = 512;
#endif

#ifdef FILE_TERRAIN
			uint8_t *heightmap = new uint8_t[f.length];
			std::memset( heightmap, 0, f.length );
			f.Read( heightmap );
#else
			uint8_t heightmap[dimensions * dimensions] = {};
			for ( int row = 0; row < dimensions; row++ ) {
				for ( int col = 0; col < dimensions; col++ ) {
					real32_t r = rand() / static_cast<real32_t>( RAND_MAX );
					heightmap[(row * dimensions) + col] = r * 255.0f;
				}
			}

#endif

			GameObject *terrainObj = new GameObject();
			terrainObj->renderObject = Renderer::Model::Register( nullptr );
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

			dynamic_cast<Renderer::Model *>( terrainObj->renderObject )->meshes.push_back( mesh );
			AddObject( terrainObj );

			// create texture
			texture = new Renderer::Texture( dimensions, dimensions, Renderer::InternalFormat::R8, heightmap );

#ifdef FILE_TERRAIN
			delete[] heightmap;
#endif

			// create shader program
			static const Renderer::VertexAttribute attributes[] = {
				{ 0, "in_position" },
				{ 1, "in_normal" },
				{ 2, "in_texCoord" },
			};
			program = new Renderer::ShaderProgram( "model", "model", attributes, ARRAY_LEN( attributes ) );

			// create material
			material = new Renderer::Material();
			Renderer::Material::SamplerBinding samplerBinding( texture );
			material->samplerBindings.push_back( samplerBinding );
			material->shaderProgram = program;
			//terrainMaterial->flags |= Renderer::MF_WIREFRAME;

			mesh->material = material;
		}

		Terrain::~Terrain() {
			delete material;
			delete program;
			delete texture;
		}

	} // namespace ClientGame

} // namespace XS
