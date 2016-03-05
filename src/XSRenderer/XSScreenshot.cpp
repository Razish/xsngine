#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSString.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBackend.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSFramebuffer.h"

namespace XS {

	namespace Renderer {

		namespace Backend {

			static const size_t numScreenshotsPerFrame = 4u;

			static const char *GetScreenshotName( void ) {
				static char timestamp[numScreenshotsPerFrame][XS_MAX_FILENAME];
				static uint32_t index = 0u;

				time_t rawtime;
				time( &rawtime );

				char *p = timestamp[index];
				//TODO: stop name clash?
				strftime( p, sizeof(timestamp[index]), "%Y-%m-%d_%H-%M-%S", localtime( &rawtime ) );

				index++;
				index &= (numScreenshotsPerFrame - 1);

				return p;
			}

			void Cmd_Screenshot( const CommandContext & context ) {
				Framebuffer::BindDefault();
				glBindBuffer( GL_PIXEL_PACK_BUFFER, defaultPbo );
				glReadPixels( 0, 0, rdState.window.width, rdState.window.height, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
				GLsync sync = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );

#ifdef _DEBUG
				SDL_assert( sync != 0 );
				const GLint error = glGetError();
				SDL_assert( error != GL_INVALID_ENUM );
				SDL_assert( error != GL_INVALID_VALUE );
#endif

				RenderCommand cmd( CommandType::Screenshot );
				std::string name = String::Format( "screenshots/%s.png", GetScreenshotName() );
				cmd.screenshot.name = name.c_str();
				cmd.screenshot.width = rdState.window.width;
				cmd.screenshot.height = rdState.window.height;
				cmd.screenshot.pbo = defaultPbo;
				cmd.screenshot.sync = sync;

				//TODO: wait until next frame
				cmd.Execute();
			}

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
