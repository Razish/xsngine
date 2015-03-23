#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSMatrix.h"
#include "XSClient/XSBaseCamera.h"
#include "XSClient/XSFlyCamera.h"
#include "XSClient/XSGameObject.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSModel.h"
#include "XSRenderer/XSMesh.h"
#include "XSRenderer/XSRenderable.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSVertexAttributes.h"
#include "XSRenderer/XSView.h"

#define CLIENT_TERRAIN

namespace XS {

	namespace ClientGame {

		static Renderer::View *sceneView = nullptr;
		static FlyCamera *camera = nullptr;
#ifdef CLIENT_TERRAIN
		static Renderer::ShaderProgram *terrainProgram = nullptr;
		static Renderer::Material *terrainMaterial = nullptr;
		static Renderer::Texture *terrainTexture = nullptr;

		// always allocate with `new`
		static std::vector<GameObject *> objects;

		static void LoadTerrain( void ) {
			const File f( "textures/terrain.raw" );
			if ( !f.open ) {
				console.Print( PrintLevel::Normal, "WARNING: Could not load terrain heightmap \"%s\"\n", f.path );
				return;
			}

			uint8_t *terrainBuf = new uint8_t[f.length];
				std::memset( terrainBuf, 0, f.length );
				f.Read( terrainBuf );

			//TODO: create mesh
			GameObject *terrainObj = new GameObject();
			terrainObj->renderObject = Renderer::Model::Register( nullptr );
			Renderer::Mesh *terrainMesh = new Renderer::Mesh();
			const size_t dimensions = sqrt( f.length );
			const real32_t scale = 64.0f;

			terrainMesh->vertices.reserve( dimensions * dimensions );
			for ( int row = 0; row < dimensions; row++ ) {
				const float fRow = static_cast<real32_t>( row );
				for ( int col = 0; col < dimensions; col++ ) {
					const float fCol = static_cast<real32_t>( col );
					uint8_t *sample = &terrainBuf[(row * dimensions) + col];
					vector3 pos;
					pos.z = fRow;
					pos.y = static_cast<real32_t>( *sample ) / scale;
					pos.x = fCol;
					terrainMesh->vertices.push_back( pos );

					vector2 uv;
					uv.x = fCol / dimensions;
					uv.y = fRow / dimensions;
					terrainMesh->UVs.push_back( uv );
				}
			}

			terrainMesh->indices.reserve( dimensions * dimensions );
			for ( int row = 0; row < dimensions - 1; row++ ) {
				for ( int col = 0; col < dimensions - 1; col++ ) {
					const int16_t start = (row * dimensions) + col;
					terrainMesh->indices.push_back( (GLshort)start );
					terrainMesh->indices.push_back( (GLshort)(start + 1) );
					terrainMesh->indices.push_back( (GLshort)(start + dimensions) );

					terrainMesh->indices.push_back( (GLshort)(start + 1) );
					terrainMesh->indices.push_back( (GLshort)(start + 1 + dimensions) );
					terrainMesh->indices.push_back( (GLshort)(start + dimensions) );
				}
			}
			terrainMesh->Upload();
			dynamic_cast<Renderer::Model *>( terrainObj->renderObject )->meshes.push_back( terrainMesh );
			objects.push_back( terrainObj );

			// create texture
			terrainTexture = new Renderer::Texture( dimensions, dimensions, Renderer::InternalFormat::R8, terrainBuf );

			delete[] terrainBuf;

			// create shader program
			static const Renderer::VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_Normal" },
				{ 2, "in_TexCoord" }
			};
			terrainProgram = new Renderer::ShaderProgram( "model", "model", attributes, ARRAY_LEN( attributes ) );

			// create material
			terrainMaterial = new Renderer::Material();
			Renderer::Material::SamplerBinding samplerBinding;
			samplerBinding.unit = 0;
			samplerBinding.texture = terrainTexture;
			terrainMaterial->samplerBindings.push_back( samplerBinding );
			terrainMaterial->shaderProgram = terrainProgram;
			terrainMaterial->flags |= Renderer::MF_WIREFRAME;

			terrainMesh->material = terrainMaterial;
		}
#endif

		static void RenderScene( real64_t dt ) {
#ifdef CAMERA_TEST
			glm::mat4 m;
			sceneView->projectionMatrix = m;
#else
			sceneView->projectionMatrix = camera->GetProjectionView();
#endif

			// the view is already bound
			for ( const auto &object : objects ) {
				sceneView->AddObject( object->renderObject );
			}
		}

		void Init( void ) {
			sceneView = new Renderer::View( 0u, 0u, false, RenderScene );
			camera = new FlyCamera();
			camera->SetFlySpeed( 0.05f );

			const glm::vec3 cameraPos( -2.0f, 0.0f, 0.0f );
			const glm::vec3 lookAt( 0.0f, 0.0f, 0.0f );
			const glm::vec3 up( 0.0f, 1.0f, 0.0f );

			Cvar *r_zRange = Cvar::Get( "r_zRange" );
			real32_t zNear = r_zRange->GetReal32( 0 );
			real32_t zFar = r_zRange->GetReal32( 1 );
			camera->SetupPerspective( glm::pi<double>() * 0.25, Renderer::state.window.aspectRatio, zNear, zFar );
			camera->LookAt( cameraPos, lookAt, up );

#ifdef CLIENT_TERRAIN
			LoadTerrain();
#endif
		//	GameObject *plane = new GameObject();
		//	plane->renderObject = Renderer::Model::Register( "models/plane.xmf" );
		//	objects.push_back( plane );

		//	GameObject *box = new GameObject();
		//	box->renderObject = Renderer::Model::Register( "models/box.xmf" );
		//	objects.push_back( box );
		}

		void Shutdown( void ) {
#ifdef CLIENT_TERRAIN
			delete terrainMaterial;
			delete terrainProgram;
			delete terrainTexture;
#endif
			delete sceneView;
			for ( auto object : objects ) {
				delete object;
			}
		}

		void RunFrame( real64_t dt ) {
			camera->Update( dt );
		}

		void DrawFrame( void ) {
			sceneView->Bind();
		}

	} // namespace ClientGame

} // namespace XS
