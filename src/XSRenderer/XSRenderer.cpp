#include "XSSystem/XSInclude.h"
#include "XSSystem/XSPlatform.h"

#include "GLEW/GL/glew.h"
#include "SDL2/SDL.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSError.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSFont.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSFramebuffer.h"
#include "XSRenderer/XSBackend.h"

namespace XS {

	namespace Renderer {

		static SDL_Window *window;
		static SDL_GLContext context;

		static Cvar *r_multisample;
		static Cvar *r_swapInterval;
		static Cvar *vid_height;
		static Cvar *vid_noBorder;
		static Cvar *vid_width;

		std::vector<View*> views;
		static View *currentView = NULL;

	#ifdef _DEBUG
		static const char *GLErrSeverityToString( GLenum severity ) {
			switch ( severity ) {
				case GL_DEBUG_SEVERITY_HIGH_ARB: return "High";
				case GL_DEBUG_SEVERITY_MEDIUM_ARB: return "Medium";
				case GL_DEBUG_SEVERITY_LOW_ARB: return "Low";
				default: return "?";
			}
		}

		static const char *GLErrSourceToString( GLenum source ) {
			switch ( source ) {
				case GL_DEBUG_SOURCE_API_ARB: return "API";
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: return "WS";
				case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: return "GLSL";
				case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: return "3rd";
				case GL_DEBUG_SOURCE_APPLICATION_ARB: return "App";
				case GL_DEBUG_SOURCE_OTHER_ARB: return "Other";
				default: return "?";
			}
		}

		static const char *GLErrTypeToString( GLenum type ) {
			switch ( type ) {
				case GL_DEBUG_TYPE_ERROR_ARB: return "Error";
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: return "Deprecated";
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: return "UB";
				case GL_DEBUG_TYPE_PORTABILITY_ARB: return "Portability";
				case GL_DEBUG_TYPE_PERFORMANCE_ARB: return "Performance";
				case GL_DEBUG_TYPE_OTHER_ARB: return "Other";
				default: return "?";
			}
		}

		static void OnGLError( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
			const GLchar *message, GLvoid *userParam )
		{
			Console::Print( "[%s] [%s] %s: %s\n", GLErrSeverityToString( severity ), GLErrSourceToString( source ),
				GLErrTypeToString( type ), message );
		}
	#endif

		void CheckGLErrors( const char *filename, int line ) {
			unsigned int error = glGetError();
			if ( error != GL_NO_ERROR ) {
				switch ( error ) {
				case GL_INVALID_ENUM:
					Console::Print( "GL_INVALID_ENUM in file %s:%d.\n", filename, line );
					break;
				case GL_INVALID_VALUE:
					Console::Print( "GL_INVALID_VALUE in file %s:%d.\n", filename, line );
					break;
				case GL_INVALID_OPERATION:
					Console::Print( "GL_INVALID_OPERATION in file %s:%d.\n", filename, line );
					break;
				case GL_STACK_OVERFLOW:
					Console::Print( "GL_STACK_OVERFLOW in file %s:%d.\n", filename, line );
					break;
				case GL_STACK_UNDERFLOW:
					Console::Print( "GL_STACK_UNDERFLOW in file %s:%d.\n", filename, line );
					break;
				case GL_OUT_OF_MEMORY:
					Console::Print( "GL_OUT_OF_MEMORY in file %s:%d.\n", filename, line );
					break;
				default:
					Console::Print( "Error code 0x%X on line %d.\n", error, line );
					break;
				}
			}
		}

		void Init( void ) {
			RegisterCvars();

			CreateDisplay();

			glewExperimental = GL_TRUE;
			GLenum error = glewInit();
			if ( error != GLEW_OK )
				throw( XSError( String::Format( "Failed to initialise GLEW: %s\n", glewGetErrorString( error ) ).c_str() ) );

		#ifdef _DEBUG
			if ( GLEW_ARB_debug_output ) {
				glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB );
				glDebugMessageCallbackARB( OnGLError, NULL );
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

			if ( vid_noBorder->GetInt() )
				windowFlags |= SDL_WINDOW_BORDERLESS;

			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );

			int multisample = r_multisample->GetInt();
			if ( multisample ) {
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, multisample );
			}
			else
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 0 );

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
			context = NULL;

			SDL_DestroyWindow( window );
			window = NULL;

			SDL_Quit();
		}

		void Update( void ) {
			glClearColor( 0.5f, 0.125f, 0.125f, 1.0f );
			glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

			for ( const auto &view : views ) {
				view->PreRender();
				for ( const auto &cmd : view->renderCommands )
					cmd.Execute();
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
