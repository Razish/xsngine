#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSString.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSModel.h"
#include "XSRenderer/XSRenderable.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSVertexAttributes.h"
#include "XSRenderer/XSView.h"

namespace XS {

	namespace ClientGame {

		static Renderer::View *sceneView = nullptr;
#if 0
		static Renderer::ShaderProgram *terrainProgram = nullptr;
		static Renderer::Material *terrainMaterial = nullptr;
		static Renderer::Texture *terrainTexture = nullptr;

		static void LoadTerrain( void ) {
			const File &f = File( "textures/terrain.raw" );
			if ( !f.open ) {
				console.Print( "WARNING: Could not load terrain heightmap \"%s\"\n", f.path );
				return;
			}

			byte *terrainBuf = new byte[f.length];
			std::memset( terrainBuf, 0, f.length );
			f.Read( terrainBuf );

			const int textureSize = static_cast<int>( sqrtf( static_cast<float>( f.length ) ) );

			// create shader program
			static const Renderer::VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_TexCoord" },
				{ 2, "in_Color" }
			};

			terrainProgram = new Renderer::ShaderProgram( "terrain", "terrain", attributes, ARRAY_LEN( attributes ) );

			// create texture
			terrainTexture = new Renderer::Texture( textureSize, textureSize, Renderer::InternalFormat::R8,
				terrainBuf );

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

		void Init( void ) {
			const uint32_t width = Cvar::Get( "vid_width" )->GetInt();
			const uint32_t height = Cvar::Get( "vid_height" )->GetInt();
			sceneView = new Renderer::View( width, height, false );

#if 0
			LoadTerrain();
#else
			GameObject box;
			box.renderObject = Renderer::Model::Register( "models/box.obj" );
			sceneView->AddObject( box.renderObject );
#endif
		}

		void RunFrame( void ) {
			// ...
		}

		void DrawFrame( void ) {
			// ...
		}

	} // namespace ClientGame

} // namespace XS
