#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSMatrix.h"
#include "XSClient/XSBaseCamera.h"
#include "XSClient/XSFlyCamera.h"
#include "XSClient/XSGameObject.h"
#include "XSClient/XSTerrain.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSModel.h"

namespace XS {

	namespace ClientGame {

		static Cvar *cg_fov = nullptr;

		static Renderer::View *sceneView = nullptr;
		static FlyCamera *camera = nullptr;
		static Terrain *terrain = nullptr;

		// always allocate with `new`
		static std::vector<GameObject *> objects;

		void AddObject( GameObject *obj ) {
			objects.push_back( obj );
		}

		static void RegisterCvars( void ) {
			//fov = degrees( pi * 0.75 ) / 1.25 because math
			cg_fov = Cvar::Create( "cg_fov", "108", "Field of view", CVAR_ARCHIVE );
		}

		static void RenderScene( real64_t dt ) {
			sceneView->projectionMatrix = camera->GetProjectionView();
		}

		void Init( void ) {
			RegisterCvars();

			sceneView = new Renderer::View( 0u, 0u, false, RenderScene );
			camera = new FlyCamera();
			camera->SetFlySpeed( 0.05f );

			const glm::vec3 cameraPos( 0.0f, 0.0f, 0.0f );
			const glm::vec3 lookAt( 0.0f, 0.0f, -1.0f );
			const glm::vec3 up( 0.0f, 1.0f, 0.0f );

			Cvar *r_zRange = Cvar::Get( "r_zRange" );
			real32_t zNear = r_zRange->GetReal32( 0 );
			real32_t zFar = r_zRange->GetReal32( 1 );

			camera->SetupPerspective( glm::radians( cg_fov->GetReal32() ), Renderer::state.window.aspectRatio, zNear, zFar );
			camera->LookAt( cameraPos, lookAt, up );

			terrain = new Terrain( "textures/terrain.raw" );

			GameObject *plane = new GameObject();
			plane->renderObject = Renderer::Model::Register( "models/plane.xmf" );
			objects.push_back( plane );

		//	GameObject *box = new GameObject();
		//	box->renderObject = Renderer::Model::Register( "models/box.xmf" );
		//	box->SetPosition( vector3( 0.0f, 0.0f, 0.0f ) );
		//	objects.push_back( box );
		}

		void Shutdown( void ) {
			delete terrain;
			delete sceneView;
			for ( auto object : objects ) {
				delete object;
			}
		}

		void RunFrame( real64_t dt ) {
			Cvar *r_zRange = Cvar::Get( "r_zRange" );
			real32_t zNear = r_zRange->GetReal32( 0 );
			real32_t zFar = r_zRange->GetReal32( 1 );

			camera->SetupPerspective( glm::radians( cg_fov->GetReal32() ), Renderer::state.window.aspectRatio, zNear, zFar );
			camera->Update( dt );
		}

		void DrawFrame( void ) {
			static const vector3 lightPos( 0.0f, 32.0f, 0.0f );
			for ( const auto &object : objects ) {
				sceneView->AddObject( object->renderObject );
			}
			sceneView->AddPointLight( lightPos );
		}

	} // namespace ClientGame

} // namespace XS
