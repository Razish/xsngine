#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSCommand.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBackend.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSRenderCommand.h"

namespace XS {

	namespace Renderer {

		namespace Backend {

			static const uint32_t numScreenshotsPerFrame = 4u;

			static const char *GetScreenshotName( void ) {
				static char timestamp[numScreenshotsPerFrame][1024];
				time_t rawtime;
				time( &rawtime );
				static uint32_t index = 0u;
				char *p = timestamp[index];
				strftime( p, sizeof(timestamp[index]), "%Y-%m-%d_%H-%M-%S.png", localtime( &rawtime ) );
				index++;
				index &= numScreenshotsPerFrame;
				return p;
			}

			void Cmd_Screenshot( const commandContext_t * const context ) {
				const int32_t w = vid_width->GetInt(), h = vid_height->GetInt();
				glBindBuffer( GL_PIXEL_PACK_BUFFER, defaultPbo );
				glReadPixels( 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
				GLsync sync = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );

				RenderCommand cmd( RenderCommand::Type::SCREENSHOT );
				cmd.screenshot.name = GetScreenshotName();
				cmd.screenshot.width = w;
				cmd.screenshot.height = h;
				cmd.screenshot.pbo = defaultPbo;
				cmd.screenshot.sync = sync;

				//TODO: wait until next frame
				cmd.Execute();
			}

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
