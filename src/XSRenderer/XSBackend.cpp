#include "XSSystem/XSInclude.h"

#include <GL/glew.h>

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

			static GLuint defaultVao;

			static void RegisterCvars( void ) {
				r_fov = Cvar::Create( "r_fov", "110", "Field of view", CVAR_ARCHIVE );
			}

			void Init( void ) {
				RegisterCvars();

				glClearColor( 0.5f, 0.125f, 0.125f, 1.0f );
				glClear( GL_COLOR_BUFFER_BIT );
				glClearDepth( 1.0f );

				// state changes
				glEnable( GL_DEPTH_TEST );
				glDepthFunc( GL_LEQUAL );

				// back-face culling
				glEnable( GL_CULL_FACE );
				glCullFace( GL_BACK );
				glFrontFace( GL_CCW );

				// alpha blending
				glEnable( GL_BLEND );
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

				// activate the default vertex array object
				glGenVertexArrays (1, &defaultVao);
				glBindVertexArray (defaultVao);
			}

			void Shutdown( ) {
				glDeleteVertexArrays (1, &defaultVao);
			}

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
