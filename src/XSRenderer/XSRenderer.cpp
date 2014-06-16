#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSBackend.h"
#include "XSRenderer/XSFramebuffer.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSFont.h"

namespace XS {

	namespace Renderer {

		static SDL_Window *window = nullptr;
		static SDL_GLContext context;

		static Cvar *r_multisample = nullptr;
		static Cvar *r_skipRender = nullptr;
		static Cvar *r_swapInterval = nullptr;
		static Cvar *vid_height = nullptr;
		static Cvar *vid_noBorder = nullptr;
		static Cvar *vid_width = nullptr;

		std::vector<View *> views;
		static View *currentView = nullptr;

	#ifdef _DEBUG
		static const char *GLErrSeverityToString( GLenum severity ) {
			switch ( severity ) {
			case GL_DEBUG_SEVERITY_HIGH_ARB:
				return "High";

			case GL_DEBUG_SEVERITY_MEDIUM_ARB:
				return "Medium";

			case GL_DEBUG_SEVERITY_LOW_ARB:
				return "Low";

			default:
				return "?";
			}
		}

		static const char *GLErrSourceToString( GLenum source ) {
			switch ( source ) {
			case GL_DEBUG_SOURCE_API_ARB:
				return "API";

			case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
				return "WS";

			case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
				return "GLSL";

			case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
				return "3rd";

			case GL_DEBUG_SOURCE_APPLICATION_ARB:
				return "App";

			case GL_DEBUG_SOURCE_OTHER_ARB:
				return "Other";

			default:
				return "?";
			}
		}

		static const char *GLErrTypeToString( GLenum type ) {
			switch ( type ) {
			case GL_DEBUG_TYPE_ERROR_ARB:
				return "Error";

			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
				return "Deprecated";

			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
				return "UB";

			case GL_DEBUG_TYPE_PORTABILITY_ARB:
				return "Portability";

			case GL_DEBUG_TYPE_PERFORMANCE_ARB:
				return "Performance";

			case GL_DEBUG_TYPE_OTHER_ARB:
				return "Other";

			default:
				return "?";
			}
		}

		static void OnGLError( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
			const GLchar *message, GLvoid *userParam )
		{
			Console::Print( "[%s] [%s] %s: %s\n", GLErrSeverityToString( severity ), GLErrSourceToString( source ),
				GLErrTypeToString( type ), message );
		}
	#endif

		void Init( void ) {
			RegisterCvars();

			CreateDisplay();

			glewExperimental = GL_TRUE;
			GLenum error = glewInit();
			if ( error != GLEW_OK ) {
				throw( XSError( String::Format( "Failed to initialise GLEW: %s\n",
					glewGetErrorString( error ) ).c_str() ) );
			}

		#ifdef _DEBUG
			if ( GLEW_ARB_debug_output ) {
				glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB );
				glDebugMessageCallbackARB( OnGLError, nullptr );
			}
		#endif

			Backend::Init();

			Texture::Init();
			ShaderProgram::Init();
			Framebuffer::Init();
			Font::Init();

			RenderCommand::Init();

			glViewport( 0, 0, vid_width->GetInt(), vid_height->GetInt() );
		}

		void Shutdown( void ) {
			Console::Print( "Shutting down renderer...\n" );

			RenderCommand::Shutdown();
			Font::Shutdown();
			Backend::Shutdown();

			DestroyDisplay();
		}

		void RegisterCvars( void ) {
			r_multisample = Cvar::Create( "r_multisample", "2", "Multisample Anti-Aliasing (MSAA) level", CVAR_ARCHIVE );
			r_skipRender = Cvar::Create( "r_skipRender", "0", "1 - skip 3D views, 2 - skip 2D views, 3 - skip all views",
				CVAR_ARCHIVE );
			r_swapInterval = Cvar::Create( "r_swapInterval", "0", "Enable vertical sync", CVAR_ARCHIVE );
			vid_height = Cvar::Create( "vid_height", "720", "Window height", CVAR_ARCHIVE );
			vid_noBorder = Cvar::Create( "vid_noBorder", "0", "Disable window border", CVAR_ARCHIVE );
			vid_width = Cvar::Create( "vid_width", "1280", "Window width", CVAR_ARCHIVE );
		}

		void CreateDisplay( void ) {
			Uint32 windowFlags = SDL_WINDOW_OPENGL;

			if ( SDL_Init( SDL_INIT_VIDEO ) != 0 ) {
				return;
			}

			if ( vid_noBorder->GetBool() ) {
				windowFlags |= SDL_WINDOW_BORDERLESS;
			}

			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );

			int multisample = r_multisample->GetInt();
			if ( multisample ) {
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, multisample );
			}
			else {
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 0 );
			}

		#ifdef _DEBUG
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );
		#endif

			window = SDL_CreateWindow( WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				vid_width->GetInt(), vid_height->GetInt(), windowFlags );
			context = SDL_GL_CreateContext( window );

			SDL_GL_MakeCurrent( window, context );

			SDL_GL_SetSwapInterval( r_swapInterval->GetInt() );

			Console::Print( "OpenGL device: %s %s\n", glGetString( GL_VENDOR ), glGetString( GL_RENDERER ) );
			Console::Print( "OpenGL version: %s with GLSL %s\n", glGetString( GL_VERSION ),
				glGetString( GL_SHADING_LANGUAGE_VERSION ) );
		}

		void DestroyDisplay( void ) {
			SDL_GL_DeleteContext( context );
			context = nullptr;

			SDL_DestroyWindow( window );
			window = nullptr;

			SDL_Quit();
		}

		void Update( void ) {

			glClearColor( 0.5f, 0.125f, 0.125f, 1.0f );
			glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

			for ( const auto &view : views ) {
				if ( r_skipRender->GetInt() & (1<<static_cast<uint32_t>(view->is2D)) ) {
					continue;
				}

				view->PreRender();
				for ( const auto &cmd : view->renderCommands ) {
					cmd.Execute();
				}
				view->PostRender();
				view->renderCommands.clear();
			}

			SDL_GL_SwapWindow( window );
		}

		void RegisterView( View *view ) {
			views.push_back( view );
		}

		void SetView( View *view ) {
			currentView = view;
		}

		void DrawQuad( float x, float y, float w, float h, float s1, float t1, float s2, float t2,
			const Material& material )
		{
			RenderCommand cmd( RenderCommand::DRAWQUAD );
			cmd.drawQuad.x = x;
			cmd.drawQuad.y = y;
			cmd.drawQuad.w = w;
			cmd.drawQuad.h = h;
			cmd.drawQuad.s1 = s1;
			cmd.drawQuad.t1 = t1;
			cmd.drawQuad.s2 = s2;
			cmd.drawQuad.t2 = t2;
			cmd.drawQuad.material = &material;

			currentView->renderCommands.push_back( cmd );
		}

	} // namespace Renderer

} // namespace XS
