#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSCvar.h"
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

		Cvar *r_clear = nullptr;
		Cvar *r_debug = nullptr;
		Cvar *r_multisample = nullptr;
		Cvar *r_skipRender = nullptr;
		Cvar *r_swapInterval = nullptr;
		Cvar *vid_height = nullptr;
		Cvar *vid_noBorder = nullptr;
		Cvar *vid_width = nullptr;

		std::vector<View *> views;
		static View *currentView = nullptr;

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
			const int level = r_debug->GetInt();
			if ( !level || (level == 1 && type == GL_DEBUG_TYPE_PERFORMANCE_ARB) ) {
				return;
			}

			console.Print( "[%s] [%s] %s: %s\n", GLErrSeverityToString( severity ), GLErrSourceToString( source ),
				GLErrTypeToString( type ), message );
		}

		void Init( void ) {
			RegisterCvars();

			CreateDisplay();

			glewExperimental = GL_TRUE;
			GLenum error = glewInit();
			if ( error != GLEW_OK ) {
				throw( XSError( String::Format( "Failed to initialise GLEW: %s\n",
					glewGetErrorString( error ) ).c_str() ) );
			}

			if ( GLEW_ARB_debug_output ) {
				glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB );
				glDebugMessageCallbackARB( OnGLError, nullptr );
			}

			Backend::Init();

			Texture::Init();
			ShaderProgram::Init();
			Font::Init();

			RenderCommand::Init();

			glViewport( 0, 0, vid_width->GetInt(), vid_height->GetInt() );
		}

		void Shutdown( void ) {
			console.Print( "Shutting down renderer...\n" );

			RenderCommand::Shutdown();
			Font::Shutdown();
			Backend::Shutdown();

			DestroyDisplay();
		}

		void RegisterCvars( void ) {
			r_clear = Cvar::Create( "r_clear", "0.5 0.0 0.0 1.0", "Colour of the backbuffer", CVAR_ARCHIVE );
			r_debug = Cvar::Create( "r_debug", "0", "Enable debugging information", CVAR_ARCHIVE );
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

			console.Print( "OpenGL device: %s %s\n", glGetString( GL_VENDOR ), glGetString( GL_RENDERER ) );
			console.Print( "OpenGL version: %s with GLSL %s\n", glGetString( GL_VERSION ),
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
			const vector4 clear( r_clear->GetFloat( 0 ), r_clear->GetFloat( 1 ), r_clear->GetFloat( 2 ),
				r_clear->GetFloat( 3 ) );
			glClearColor( clear.r, clear.g, clear.b, clear.a );
			glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

			for ( const auto &view : views ) {
				if ( r_skipRender->GetInt() & (1 << static_cast<uint32_t>( view->is2D )) ) {
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

		void BindView( View *view ) {
			currentView = view;
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

			RenderCommand cmd( RenderCommand::Type::DRAWQUAD );
			cmd.drawQuad.x = x;
			cmd.drawQuad.y = y;
			cmd.drawQuad.w = w;
			cmd.drawQuad.h = h;
			cmd.drawQuad.s1 = s1;
			cmd.drawQuad.t1 = t1;
			cmd.drawQuad.s2 = s2;
			cmd.drawQuad.t2 = t2;
			cmd.drawQuad.colour = colour;
			cmd.drawQuad.material = material;

			currentView->renderCommands.push_back( cmd );
		}

		void DrawModel( const Model *model ) {
			AssertView();

			RenderCommand cmd( RenderCommand::Type::DRAWMODEL );
			cmd.drawModel.model = model;
			currentView->renderCommands.push_back( cmd );
		}

	} // namespace Renderer

} // namespace XS
