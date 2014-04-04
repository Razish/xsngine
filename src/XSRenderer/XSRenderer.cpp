#include "XSSystem/XSInclude.h"
#include "XSSystem/XSPlatform.h"

#include "GLee/GLee.h"
#include "SDL2/SDL.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSVector.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
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
			Backend::Init();

			Texture::Init();
			ShaderProgram::Init();
			Framebuffer::Init();
			Font::Init();

			Backend::Begin2D( vid_width->GetInt(), vid_height->GetInt() );
		}

		void Shutdown( void ) {
			Console::Print( "Shutting down renderer...\n" );

			Font::Shutdown();

			DestroyDisplay();
		}

		void RegisterCvars( void ) {
			r_multisample			= Cvar::Create( "r_multisample", "2", CVAR_ARCHIVE );
			r_swapInterval			= Cvar::Create( "r_swapInterval", "0", CVAR_ARCHIVE );
			vid_height				= Cvar::Create( "vid_height", "720", CVAR_ARCHIVE );
			vid_noBorder			= Cvar::Create( "vid_noBorder", "0", CVAR_ARCHIVE );
			vid_width				= Cvar::Create( "vid_width", "1280", CVAR_ARCHIVE );
		}

		void CreateDisplay( void ) {
			Uint32 windowFlags = SDL_WINDOW_OPENGL;

			if ( vid_noBorder->GetInt() )
				windowFlags |= SDL_WINDOW_BORDERLESS;

			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );

			int multisample = r_multisample->GetInt();
			if ( multisample ) {
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, multisample );
			}

			window = SDL_CreateWindow( WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				vid_width->GetInt(), vid_height->GetInt(), windowFlags );
			context = SDL_GL_CreateContext( window );

			SDL_GL_SetSwapInterval( r_swapInterval->GetInt() );
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

			glLoadIdentity();

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
			const Texture *texture )
		{
			RenderCommand cmd;

			//TODO: procedurally generated default/missing texture
			if ( !texture )
				return;

			cmd.drawQuad.x = x / vid_width->GetInt();
			cmd.drawQuad.y = y / vid_height->GetInt();
			cmd.drawQuad.w = w;
			cmd.drawQuad.h = h;
			cmd.drawQuad.s1 = s1;
			cmd.drawQuad.t1 = t2;
			cmd.drawQuad.s2 = s2;
			cmd.drawQuad.t2 = t2;
			cmd.drawQuad.textureID = texture->id;

			cmd.type = RC_DRAWQUAD;

			currentView->renderCommands.push_back( cmd );
		}

	} // namespace Renderer

} // namespace XS
