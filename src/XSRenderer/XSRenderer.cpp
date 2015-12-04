#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSTimer.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSBackend.h"
#include "XSRenderer/XSFramebuffer.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSFont.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSVertexAttributes.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSBuffer.h"

#if defined(XS_OS_MAC)
//#include <OpenCL/cl_gl_ext.h>
#endif

#define USE_FBO
#define FBO_COMPOSITE

// ARB_debug_output causes a crash due to a driver bug on Nvidia GTX 680 driver versions [350.12, 353.82]
//	targeting OpenGL 3.1 core profile
//#define RENDERER_DEBUG_OUTPUT

namespace XS {

	namespace Renderer {

		RendererState rdState = {};

		static SDL_Window *window = nullptr;
		static SDL_GLContext context;

		static Cvar *r_clear = nullptr;
		Cvar *r_debug = nullptr;
		Cvar *r_fastPath = nullptr;
		static Cvar *r_multisample = nullptr;
		static Cvar *r_skipRender = nullptr;
		static Cvar *r_swapInterval = nullptr;
		static Cvar *vid_height = nullptr;
		static Cvar *vid_noBorder = nullptr;
		static Cvar *vid_width = nullptr;

		std::vector<View *> views;
		static View *currentView = nullptr;

#ifdef FBO_COMPOSITE
		static ShaderProgram *compositeShader = nullptr;
#endif

	#ifdef RENDERER_DEBUG_OUTPUT
		static const char *GLErrSeverityToString( GLenum severity ) {
			switch ( severity ) {
			case GL_DEBUG_SEVERITY_HIGH_ARB: {
				return "High";
			} break;

			case GL_DEBUG_SEVERITY_MEDIUM_ARB: {
				return "Medium";
			} break;

			case GL_DEBUG_SEVERITY_LOW_ARB: {
				return "Low";
			} break;

			default: {
				return "?";
			} break;
			}
		}

		static const char *GLErrSourceToString( GLenum source ) {
			switch ( source ) {
			case GL_DEBUG_SOURCE_API_ARB: {
				return "API";
			} break;

			case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: {
				return "WS";
			} break;

			case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: {
				return "GLSL";
			} break;

			case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: {
				return "3rd";
			} break;

			case GL_DEBUG_SOURCE_APPLICATION_ARB: {
				return "App";
			} break;

			case GL_DEBUG_SOURCE_OTHER_ARB: {
				return "Other";
			} break;

			default: {
				return "?";
			} break;
			}
		}

		static const char *GLErrTypeToString( GLenum type ) {
			switch ( type ) {
			case GL_DEBUG_TYPE_ERROR_ARB: {
				return "Error";
			} break;

			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: {
				return "Deprecated";
			} break;

			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: {
				return "UB";
			} break;

			case GL_DEBUG_TYPE_PORTABILITY_ARB: {
				return "Portability";
			} break;

			case GL_DEBUG_TYPE_PERFORMANCE_ARB: {
				return "Performance";
			} break;

			case GL_DEBUG_TYPE_OTHER_ARB: {
				return "Other";
			} break;

			default: {
				return "?";
			} break;
			}
		}

		static void OnGLError( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
			const GLchar *message, GLvoid *userParam )
		{
			const int level = r_debug->GetInt32();
			if ( !level || (level == 1 && type == GL_DEBUG_TYPE_PERFORMANCE_ARB) ) {
				return;
			}

			console.Print( PrintLevel::Normal, "[%s] [%s] %s: %s\n",
				GLErrSeverityToString( severity ),
				GLErrSourceToString( source ),
				GLErrTypeToString( type ),
				message
			);
		}
	#endif // RENDERER_DEBUG_OUTPUT

		void Init( void ) {
			rdState.valid = true;

			RegisterCvars();

			CreateDisplay();

			glewExperimental = GL_TRUE;
			GLenum error = glewInit();
			if ( error != GLEW_OK ) {
				throw( XSError( String::Format( "Failed to initialise GLEW: %s\n",
					glewGetErrorString( error ) ).c_str() ) );
			}

			// ARB_debug_output causes a crash due to a driver bug on Nvidia GTX 680 driver versions [350.12, 353.82]
			//	targeting OpenGL 3.1 core profile
		#ifdef RENDERER_DEBUG_OUTPUT
			if ( GLEW_ARB_debug_output ) {
				glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB );
				glDebugMessageCallbackARB( OnGLError, nullptr );
			}
		#endif // RENDERER_DEBUG_OUTPUT

			Backend::Init();

			Texture::Init();

			ShaderProgram::Init();

#ifdef FBO_COMPOSITE
			// create composite shader
			static const VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_TexCoord" },
				{ 2, "in_Colour" },
			};
			if ( !compositeShader ) {
				compositeShader = new ShaderProgram( "composite", "composite", attributes, ARRAY_LEN( attributes ) );
			}
#endif

			Font::Init();

			RenderCommand::Init();

			glViewport( 0, 0, rdState.window.width, rdState.window.height );
		}

		void Shutdown( void ) {
			console.Print( PrintLevel::Normal, "Shutting down renderer...\n" );

			RenderCommand::Shutdown();
			Font::Shutdown();
			Backend::Shutdown();

			DestroyDisplay();
		}

		void RegisterCvars( void ) {
			r_clear = Cvar::Create( "r_clear", "0.1607 0.1921 0.2039 1.0",
				"Colour of the backbuffer", CVAR_ARCHIVE
			);
			r_debug = Cvar::Create( "r_debug", "0",
				"Enable debugging information", CVAR_ARCHIVE
			);
			r_fastPath = Cvar::Create( "r_fastPath", XS_DEBUG_BUILD ? "0" : "1",
				"Use stable but slow, or unstable but fast render paths", CVAR_ARCHIVE
			);
			r_multisample = Cvar::Create( "r_multisample", "2",
				"Multisample Anti-Aliasing (MSAA) level", CVAR_ARCHIVE
			);
			r_skipRender = Cvar::Create( "r_skipRender", "0",
				"1 - skip 3D views, 2 - skip 2D views, 3 - skip all views", CVAR_ARCHIVE
			);
			r_swapInterval = Cvar::Create( "r_swapInterval", "0",
				"Enable vertical sync", CVAR_ARCHIVE
			);
			vid_height = Cvar::Create( "vid_height", "720",
				"Window height", CVAR_ARCHIVE
			);
			vid_noBorder = Cvar::Create( "vid_noBorder", "0",
				"Disable window border", CVAR_ARCHIVE
			);
			vid_width = Cvar::Create( "vid_width", "1280",
				"Window width", CVAR_ARCHIVE
			);
		}

		void CreateDisplay( void ) {
			Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;

			if ( SDL_Init( SDL_INIT_VIDEO ) != 0 ) {
				return;
			}

			if ( vid_noBorder->GetBool() ) {
				windowFlags |= SDL_WINDOW_BORDERLESS;
			}

			// targeting OpenGL 3.1 core
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
			uint32_t contextFlags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
			if ( Common::com_developer->GetBool() ) {
				contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
			}
#ifdef _DEBUG
			contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, contextFlags );

			int multisample = r_multisample->GetInt32();
			if ( multisample > 0 ) {
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
				//TODO: find the highest significant bit to ensure samples^2
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, multisample );
			}
			else {
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 0 );
			}

			int32_t width = vid_width->GetInt32();
			int32_t height = vid_height->GetInt32();
			window = SDL_CreateWindow(
				WINDOW_TITLE,
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				width, height,
				windowFlags
			);
			SDL_assert( window && "Failed to create window" );

			// when creating a fullscreen window, the actual width/height may not be what was requested
			// certain drawing code relies on knowing the width/height of the display, so we'll update it with the
			//      proper values
			SDL_GetWindowSize( window, &width, &height );
			vid_width->Set( width );
			vid_height->Set( height );

			context = SDL_GL_CreateContext( window );
			SDL_assert( context && "Failed to create OpenGL context on window" );

			SDL_GL_MakeCurrent( window, context );
			rdState.window.valid = true;
			rdState.window.width = static_cast<uint32_t>( width );
			rdState.window.height = static_cast<uint32_t>( height );
			rdState.window.aspectRatio = vid_width->GetReal64() / vid_height->GetReal64();

			SDL_GL_SetSwapInterval( r_swapInterval->GetInt32() );
		#if defined(XS_OS_MAC)
			//TODO: force vsync flag in CGL, seems to only have an Obj-C API?
			/*
			CGLContextObj cglContext = CGLGetCurrentContext();
			if ( cglContext ) {
				// ...
			}
			*/
		#endif

			rdState.driver.vendor = reinterpret_cast<const char *>( glGetString( GL_VENDOR ) );
			rdState.driver.renderer = reinterpret_cast<const char *>( glGetString( GL_RENDERER ) );
			rdState.driver.coreVersion = reinterpret_cast<const char *>( glGetString( GL_VERSION ) );
			rdState.driver.shaderVersion = reinterpret_cast<const char *>( glGetString( GL_SHADING_LANGUAGE_VERSION ) );

			console.Print( PrintLevel::Normal, "OpenGL device: %s %s\n",
				rdState.driver.vendor,
				rdState.driver.renderer
			);
			console.Print( PrintLevel::Normal, "OpenGL version: %s with GLSL %s\n",
				rdState.driver.coreVersion,
				rdState.driver.shaderVersion
			);
		}

		void DestroyDisplay( void ) {
			SDL_GL_DeleteContext( context );
			context = nullptr;

			SDL_DestroyWindow( window );
			window = nullptr;
			rdState.window = {};

			SDL_Quit();
		}

		void Update( real64_t dt ) {
			const vector4 clearColour = {
				r_clear->GetReal32( 0 ),
				r_clear->GetReal32( 1 ),
				r_clear->GetReal32( 2 ),
				r_clear->GetReal32( 3 )
			};

#ifdef USE_FBO
			Backend::SetBlendFunction( Backend::BlendFunc::SourceAlpha, Backend::BlendFunc::OneMinusSourceAlpha );
#else
			Backend::ClearBuffer( true, true, clearColour );
#endif

			for ( const auto &view : views ) {
				if ( r_skipRender->GetInt32() & (1 << static_cast<uint32_t>( view->is2D )) ) {
					continue;
				}

				// bind the view's FBO
				view->Bind();

#ifdef USE_FBO
				Backend::ClearBuffer( true, true, vector4{ 0.0f, 0.0f, 0.0f, 1.0f } );
#endif

				view->PreRender( dt );
				while ( !view->renderCommands.empty() ) {
					const auto &cmd = view->renderCommands.front();

					cmd.Execute();

					view->renderCommands.pop();
				}
				view->PostRender( dt );
			}

#ifdef USE_FBO
			Framebuffer::BindDefault();
			Backend::ClearBuffer( true, true, clearColour );
			Backend::SetBlendFunction( Backend::BlendFunc::One, Backend::BlendFunc::One );

			for ( const auto &view : views ) {
#ifdef FBO_COMPOSITE
				Material compositeMaterial = {};
				compositeMaterial.shaderProgram = compositeShader;

				Material::SamplerBinding colourBinding = {};
					colourBinding.unit = 0;
				//	colourBinding.uniform = "u_viewTexture";
					colourBinding.texture = const_cast<Texture *>( view->fbo->colourTextures[0] );
				compositeMaterial.samplerBindings.push_back( colourBinding );

				DrawQuadCommand cmd = {};
					cmd.pos[0] = -1.0f;
					cmd.pos[1] = 1.0f;
					cmd.size[0] = 2.0f;
					cmd.size[1] = -2.0f;
					cmd.st1[0] = 0.0f;
					cmd.st1[1] = 1.0f;
					cmd.st2[0] = 1.0f;
					cmd.st2[1] = 0.0f;
					cmd.material = &compositeMaterial;
					cmd.colour = nullptr;
				DrawQuad( cmd );
#else
				Framebuffer::BlitColour(
					view->fbo, nullptr, // from, to
					view->width, view->height, // source dimensions
					state.window.width, state.window.height // dest dimensions
				);
#endif
			}
#endif

			SDL_GL_SwapWindow( window );
		}

		//TODO: unify View code, perhaps static functions/members
		void RegisterView( View *view ) {
			views.push_back( view );
		}

		void SetCurrentView( View *view ) {
			currentView = view;
		}

		const View *GetCurrentView( void ) {
			return currentView;
		}

		static void AssertView( void ) {
			if ( !currentView ) {
				throw( XSError( "Attempted to issue render command without binding a view" ) );
			}
		}

		void DrawQuad( real32_t x, real32_t y, real32_t w, real32_t h, real32_t s1, real32_t t1, real32_t s2,
			real32_t t2, const vector4 *colour, const Material *material )
		{
			AssertView();

			RenderCommand cmd( CommandType::DrawQuad );
			cmd.drawQuad.pos[0] = x;
			cmd.drawQuad.pos[1] = y;
			cmd.drawQuad.size[0] = w;
			cmd.drawQuad.size[1] = h;
			cmd.drawQuad.st1[0] = s1;
			cmd.drawQuad.st1[1] = t1;
			cmd.drawQuad.st2[0] = s2;
			cmd.drawQuad.st2[1] = t2;
			cmd.drawQuad.colour = colour;
			cmd.drawQuad.material = material;

			currentView->renderCommands.push( cmd );
		}

		void DrawModel( const Model *model, const RenderInfo &info ) {
			AssertView();

			RenderCommand cmd( CommandType::DrawModel );
			cmd.drawModel.model = model;
			cmd.drawModel.info = info;
			currentView->renderCommands.push( cmd );
		}

		void DrawParticles( const Backend::Buffer *vbo, const Backend::Buffer *ibo, const Material *material,
			size_t count )
		{
			AssertView();

			RenderCommand cmd( CommandType::DrawParticles );
			cmd.drawParticles.vbo = vbo;
			cmd.drawParticles.ibo = ibo;
			cmd.drawParticles.material = material;
			cmd.drawParticles.count = count;
			currentView->renderCommands.push( cmd );
		}

	} // namespace Renderer

} // namespace XS
