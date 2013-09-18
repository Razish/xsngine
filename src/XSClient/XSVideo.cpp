#include "XSClient/XSVideo.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "System/XSPlatform.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace XS {

	namespace Client {

		// Client video instance. Access via Client::video.Blah()
		Video video;

		void Video::Init() {
			RegisterCvars();
			CreateDisplay();
			InitGL();
			SetViewport( vid_width->Int(), vid_height->Int() );
		}
	
		void Video::RegisterCvars( void ) {
			vid_width		= Cvar::Create( "vid_width", "1280", Cvar::ARCHIVE );
			vid_height		= Cvar::Create( "vid_height", "720", Cvar::ARCHIVE );
			vid_noBorder	= Cvar::Create( "vid_noBorder", "0", Cvar::ARCHIVE );
			r_swapInterval	= Cvar::Create( "r_swapInterval", "0", Cvar::ARCHIVE );
			r_fov			= Cvar::Create( "r_fov", "110", Cvar::ARCHIVE );
		}
	
		void Video::CreateDisplay( void ) {
			Uint32 windowFlags = SDL_WINDOW_OPENGL;
	
			if ( vid_noBorder->Int() )
				windowFlags |= SDL_WINDOW_BORDERLESS;
	
			SDL_Init( SDL_INIT_VIDEO );

			SDL_GL_SetSwapInterval( r_swapInterval->Int() );
			SDL_CreateWindowAndRenderer( vid_width->Int(), vid_height->Int(), windowFlags, &displayWindow, &displayRenderer );
			SDL_GetRendererInfo( displayRenderer, &displayRendererInfo );

			SDL_SetWindowTitle( displayWindow, WINDOW_TITLE );
	
			// https://gist.github.com/exavolt/2360410
		}
	
		void Video::InitGL( void ) {
			glShadeModel( GL_SMOOTH );
	
			glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
			glClearDepth( 1.0f );
			glEnable( GL_DEPTH_TEST );
			glDepthFunc( GL_LEQUAL );
	
			glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
		}
	
		// by Dranith http://glprogramming.com/dgs.php?dg=1
		void XS_gluPerspective( GLdouble fovx, GLdouble aspect, GLdouble zNear, GLdouble zFar ) {
			// This code is based off the MESA source for gluPerspective
			// *NOTE* This assumes GL_PROJECTION is the current matrix
	
			GLdouble xmin, xmax, ymin, ymax;
			GLdouble m[4][4] = {
				{ 0, 0, 0, 0 },
				{ 0, 0, 0, 0 },
				{ 0, 0, 0, 0 },
				{ 0, 0, 0, 0 } };
	
			xmax = zNear * tan( fovx * M_PI / 360.0 );
			xmin = -xmax;
	
			ymin = xmin / aspect;
			ymax = xmax / aspect;
	
			// Set up the projection matrix
			m[0][0] = (2.0 * zNear) / (xmax - xmin);
			m[1][1] = (2.0 * zNear) / (ymax - ymin);
			m[2][2] = -(zFar + zNear) / (zFar - zNear);
	
			m[0][2] = (xmax + xmin) / (xmax - xmin);
			m[1][2] = (ymax + ymin) / (ymax - ymin);
			m[3][2] = -1.0;
	
			m[2][3] = -(2.0 * zFar * zNear) / (zFar - zNear);
	
			// Add to current matrix
			glMultMatrixd( (GLdouble *)m );
		}
	
		void Video::SetViewport( const int width, const int height ) {
			GLfloat ratio;
	
			ratio = (GLfloat)width / (GLfloat)height;
	
			glViewport( 0, 0, (GLsizei)width, (GLsizei)height );
	
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
	
			XS_gluPerspective( r_fov->Float(), ratio, 0.1, 1000.0 );
	
			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();
		}
	
		void Video::Update( void ) {
			glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
			glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
	
			glLoadIdentity();
	
			glTranslatef( -1.5f, 0.0f, -6.0f );
			glBegin( GL_TRIANGLES );
				glVertex3f(  0.0f,  1.0f, 0.0f );
				glVertex3f( -1.0f, -1.0f, 0.0f );
				glVertex3f(  1.0f, -1.0f, 0.0f );
			glEnd();
	
			glTranslatef( 3.0f, 0.0f, 0.0f );
			glBegin( GL_QUADS );
				glVertex3f( -1.0f,  1.0f, 0.0f );
				glVertex3f(  1.0f,  1.0f, 0.0f );
				glVertex3f(  1.0f, -1.0f, 0.0f );
				glVertex3f( -1.0f, -1.0f, 0.0f );
			glEnd();
	
			SDL_RenderPresent( displayRenderer );
		}

	}; // namespace Client

}; // namespace XS
