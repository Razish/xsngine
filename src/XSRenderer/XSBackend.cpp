#include "XSSystem/XSInclude.h"

#include "GLee/GLee.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Renderer {

		namespace Backend {

			static Cvar *r_fov;

			static bool in2D = false;

			static void RegisterCvars( void ) {
				r_fov = Cvar::Create( "r_fov", "110", "Field of view", CVAR_ARCHIVE );
			}

			void Init( void ) {
				RegisterCvars();

				glClearColor( 0.5f, 0.125f, 0.125f, 1.0f );
				glClear( GL_COLOR_BUFFER_BIT );
				glClearDepth( 1.0f );

				// state changes
				glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

				glEnable( GL_DEPTH_TEST );
				glDepthFunc( GL_LEQUAL );

				glEnable( GL_TEXTURE_2D );

				// back-face culling
				glEnable( GL_CULL_FACE );
				glCullFace( GL_BACK );
				glFrontFace( GL_CCW );

				// alpha blending
				glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

				// rendering primitives
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableClientState( GL_TEXTURE_COORD_ARRAY );
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

			void Begin2D( const int width, const int height ) {
				if ( in2D )
					return;
				in2D = true;

				glDisable( GL_DEPTH_TEST );
				glDisable( GL_CULL_FACE );

				glMatrixMode( GL_PROJECTION );
			//	glPushMatrix();
				glLoadIdentity();
				glOrtho( 0.0, (double)width, (double)height, 0.0, 0.0, 1.0 );

				glMatrixMode( GL_MODELVIEW );
				glLoadIdentity();

				// state changes
				glDisable( GL_CULL_FACE );

				glEnable( GL_TEXTURE_2D );

				glClear( GL_DEPTH_BUFFER_BIT );
				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			}

			void Begin3D( const int width, const int height ) {
				if ( !in2D )
					return;
				in2D = false;

				glMatrixMode( GL_PROJECTION );
				glLoadIdentity();

				glMatrixMode( GL_MODELVIEW );
				glLoadIdentity();

				GLfloat ratio = (GLfloat)width / (GLfloat)height;
				XS_gluPerspective( r_fov->GetFloat(), ratio, 0.1, 1000.0 );

				glViewport( 0, 0, (GLsizei)width, (GLsizei)height );

				// state changes
				glEnable( GL_CULL_FACE );
			}

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
