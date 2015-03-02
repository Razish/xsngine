#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSMatrix.h"
#include "XSClient/XSCamera.h"
#include "XSClient/XSCamera2.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSModel.h"
#include "XSRenderer/XSRenderable.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSVertexAttributes.h"
#include "XSRenderer/XSView.h"

namespace XS {

	namespace ClientGame {

//#define USE_CAMERA1

		static Renderer::View *sceneView = nullptr;
#ifdef USE_CAMERA1
		static Camera *camera = nullptr;
#else
		static Camera2 *camera = nullptr;
#endif
#ifdef CLIENT_TERRAIN
		static Renderer::ShaderProgram *terrainProgram = nullptr;
		static Renderer::Material *terrainMaterial = nullptr;
		static Renderer::Texture *terrainTexture = nullptr;

		static void LoadTerrain( void ) {
			const File f( "textures/terrain.raw" );
			if ( !f.open ) {
				console.Print( "WARNING: Could not load terrain heightmap \"%s\"\n", f.path );
				return;
			}

			byte *terrainBuf = new byte[f.length];
				std::memset( terrainBuf, 0, f.length );
				f.Read( terrainBuf );
			delete[] terrainBuf;

			const uint32_t textureSize = static_cast<uint32_t>( sqrtf( static_cast<real32_t>( f.length ) ) );

			// create texture
			terrainTexture = new Renderer::Texture( textureSize, textureSize, Renderer::InternalFormat::R8,
				terrainBuf );

			// create shader program
			static const Renderer::VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_TexCoord" },
				{ 2, "in_Colour" }
			};
			terrainProgram = new Renderer::ShaderProgram( "terrain", "terrain", attributes, ARRAY_LEN( attributes ) );

			// create material
			terrainMaterial = new Renderer::Material();
			Renderer::Material::SamplerBinding samplerBinding;
			samplerBinding.unit = 0;
			samplerBinding.texture = terrainTexture;
			terrainMaterial->samplerBindings.push_back( samplerBinding );
			terrainMaterial->shaderProgram = terrainProgram;
		}
#endif

		class GameObject {
		private:
			// ...

		public:
			Renderer::Renderable *renderObject;

			GameObject()
			: renderObject( nullptr )
			{
			};

			~GameObject() {
				if ( renderObject ) {
					delete renderObject;
					renderObject = nullptr;
				}
			};
		};

		// always allocate with `new`
		static std::vector<GameObject *> objects;

		static void RenderScene( void ) {
#ifdef USE_CAMERA1
			const vector3 cameraPos( -128.0f, -128.0f, -128.0f );
			const vector3 lookAt( 0.0f, 0.0f, 0.0f );
			const vector3 up( 0.0f, 0.0f, 1.0f );

			camera->SetupPerspective( 90.0f, 1.77777f, 0.1f, 4000.0f );
			camera->LookAt( cameraPos, lookAt, up );

			sceneView->projectionMatrix = camera->GetProjectionView();
#else
			const vector3 cameraPos( -16.0f, -16.0f, 16.0f );
			const vector3 lookAt( 0.0f, 1.0f, 0.0f );

			camera->SetupPerspective( 90.0f, 1.77777f, 0.1f, 4000.0f );
			camera->Set( cameraPos, lookAt );
			sceneView->projectionMatrix = camera->GetProjectionView();
#endif

			// the view is already bound
			for ( const auto &object : objects ) {
				sceneView->AddObject( object->renderObject );
			}
		}

		void Init( void ) {
			sceneView = new Renderer::View( 0u, 0u, false, RenderScene );
#ifdef USE_CAMERA1
			camera = new Camera();
#else
			camera = new Camera2();
#endif

#ifdef CLIENT_TERRAIN
			LoadTerrain();
#else
			GameObject *box = new GameObject();
			objects.push_back( box );
			box->renderObject = Renderer::Model::Register( "models/box.xmf" );
#endif
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

		void RunFrame( void ) {
			// ...
		}

		void DrawFrame( void ) {
			sceneView->Bind();
		}

	} // namespace ClientGame

} // namespace XS
