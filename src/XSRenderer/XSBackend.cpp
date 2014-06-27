#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSTimer.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBackend.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSRenderCommand.h"

namespace XS {

	namespace Renderer {

		namespace Backend {

			Cvar *r_fov = nullptr;
			Cvar *r_zRange = nullptr;

			static GLuint defaultVao = 0u;
			static GLuint defaultPbo = 0u;

			static void RegisterCvars( void ) {
				r_fov = Cvar::Create( "r_fov", "110", "Field of view", CVAR_ARCHIVE );
				r_zRange = Cvar::Create( "r_zRange", "4.0 1000.0", "Clipping plane range", CVAR_ARCHIVE );
			}

			static const char *GetScreenshotName( void ) {
				static char timestamp[4][1024];
				time_t rawtime;
				time( &rawtime );
				static size_t index = 0u;
				char *p = timestamp[index];
				strftime( p, sizeof(timestamp[index]), "%Y-%m-%d_%H-%M-%S.png", localtime( &rawtime ) );
				index++;
				index &= 4;
				return p;
			}

			void Cmd_Screenshot( const commandContext_t * const context ) {
				const int w = vid_width->GetInt(), h = vid_height->GetInt();
				glBindBuffer( GL_PIXEL_PACK_BUFFER, defaultPbo );
				glReadPixels( 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
				GLsync sync = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );

				RenderCommand cmd( RenderCommand::SCREENSHOT );
				cmd.screenshot.name = GetScreenshotName();
				cmd.screenshot.width = w;
				cmd.screenshot.height = h;
				cmd.screenshot.pbo = defaultPbo;
				cmd.screenshot.sync = sync;

				//TODO: wait until next frame
				cmd.Execute();
			}

			void Init( void ) {
				RegisterCvars();
				Command::AddCommand( "screenshot", Cmd_Screenshot );

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
				glGenVertexArrays( 1, &defaultVao );
				glBindVertexArray( defaultVao );

				glGenBuffers( 1, &defaultPbo );
				glBindBuffer( GL_PIXEL_PACK_BUFFER, defaultPbo );
				const int w = vid_width->GetInt(), h = vid_height->GetInt();
				glBufferData( GL_PIXEL_PACK_BUFFER, 4 * w * h, NULL, GL_STREAM_COPY );
			}

			void Shutdown( void ) {
				glDeleteVertexArrays( 1, &defaultVao );
				glDeleteBuffers( 1, &defaultPbo );
			}

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
