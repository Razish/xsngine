#include "XSSystem/XSInclude.h"

#include "GLee/GLee.h"
#include "SDL2/SDL.h"

#include "XSSystem/XSPlatform.h"
#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSVector.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSFont.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSFramebuffer.h"

namespace XS {

	namespace Renderer {

		static SDL_Window *window;
		static SDL_GLContext context;

		static Cvar *r_fov;
		static Cvar *r_multisample;
		static Cvar *r_swapInterval;
		static Cvar *vid_height;
		static Cvar *vid_noBorder;
		static Cvar *vid_width;

		void CheckGLErrors( const char *filename, int line ) {
			unsigned int error = glGetError();
			if ( error != GL_NO_ERROR ) {
				switch ( error ) {
				case GL_INVALID_ENUM:
					Print( "GL_INVALID_ENUM in file %s:%d.\n", filename, line );
					break;
				case GL_INVALID_VALUE:
					Print( "GL_INVALID_VALUE in file %s:%d.\n", filename, line );
					break;
				case GL_INVALID_OPERATION:
					Print( "GL_INVALID_OPERATION in file %s:%d.\n", filename, line );
					break;
				case GL_STACK_OVERFLOW:
					Print( "GL_STACK_OVERFLOW in file %s:%d.\n", filename, line );
					break;
				case GL_STACK_UNDERFLOW:
					Print( "GL_STACK_UNDERFLOW in file %s:%d.\n", filename, line );
					break;
				case GL_OUT_OF_MEMORY:
					Print( "GL_OUT_OF_MEMORY in file %s:%d.\n", filename, line );
					break;
				default:
					Print( "Error code 0x%X on line %d.\n", error, line );
					break;
				}
			}
		}

		void Init( void ) {
			RegisterCvars();

			CreateDisplay();
			InitGL();

			Texture::Init();
			ShaderProgram::Init();
			Framebuffer::Init();
			Font::Init();

			SetViewport( vid_width->GetInt(), vid_height->GetInt() );
		}

		void Shutdown( void ) {
			Print( "Shutting down renderer...\n" );

			DestroyDisplay();
		}
	
		void RegisterCvars( void ) {
			r_fov					= Cvar::Create( "r_fov", "110", Cvar::ARCHIVE );
			r_multisample			= Cvar::Create( "r_multisample", "2", Cvar::ARCHIVE );
			r_swapInterval			= Cvar::Create( "r_swapInterval", "0", Cvar::ARCHIVE );
			vid_height				= Cvar::Create( "vid_height", "720", Cvar::ARCHIVE );
			vid_noBorder			= Cvar::Create( "vid_noBorder", "0", Cvar::ARCHIVE );
			vid_width				= Cvar::Create( "vid_width", "1280", Cvar::ARCHIVE );
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

			window = SDL_CreateWindow( WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, vid_width->GetInt(), vid_height->GetInt(), windowFlags );
			context = SDL_GL_CreateContext( window );

			SDL_GL_SetSwapInterval( r_swapInterval->GetInt() );
		}

		void DestroyDisplay( void ) {
			SDL_GL_DeleteContext( context );
		}
	
		void InitGL( void ) {

			glShadeModel( GL_SMOOTH );
	
			glClearColor( 0.5f, 0.125f, 0.125f, 1.0f );
			glClear( GL_COLOR_BUFFER_BIT );
			glClearDepth( 1.0f );
			glEnable( GL_DEPTH_TEST );
			glDepthFunc( GL_LEQUAL );
	
			glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
		}
	
		static void XS_gluPerspective( GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar ) {
			double f = 1.0 / tan(fovy * M_PI / 360);  // convert degrees to radians and divide by 2
			double xform[16] = {
				f / aspect, 0, 0, 0,
				0,          f, 0, 0,
				0,          0, (zFar + zNear)/(zFar - zNear), -1,
				0,          0, 2*zFar*zNear/(zFar - zNear), 0
			};
			glMultMatrixd( xform );
		}
	
		void SetViewport( const int width, const int height ) {
			GLfloat ratio;
	
			ratio = (GLfloat)width / (GLfloat)height;
	
			glViewport( 0, 0, (GLsizei)width, (GLsizei)height );
	
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
	
			XS_gluPerspective( r_fov->GetFloat(), ratio, 0.1, 1000.0 );
	
			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();
		}
	
		static const GLfloat triangle[] = {
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			 0.0f,  1.0f, 0.0f,
		};

		void Update( void ) {
			glClearColor( 0.5f, 0.125f, 0.125f, 1.0f );
			glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
	
			glLoadIdentity();

			// ugly hacks
			static bool loaded = false;
			static GLuint vbo;
			static GLuint vao;
			if ( !loaded ) {
				glGenVertexArrays( 1, &vao );
				glBindVertexArray( vao );

				glGenBuffers( 1, &vbo );
				glBindBuffer( GL_ARRAY_BUFFER, vbo );
				glBufferData( GL_ARRAY_BUFFER, sizeof( triangle ), triangle, GL_STATIC_DRAW );
				loaded = true;
			}

			glEnableVertexAttribArray( 0 );
			glBindBuffer( GL_ARRAY_BUFFER, vbo );
			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
			glDrawArrays( GL_TRIANGLES, 0, 3 );
			glDisableVertexAttribArray( 0 );

			SDL_GL_SwapWindow( window );
		}

	}; // namespace Renderer

}; // namespace XS
