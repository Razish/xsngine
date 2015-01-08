#pragma once

#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Renderer {

		struct Material;
		class Model;

		struct rcDrawQuad_t {
			float x, y;
			float w, h;
			float s1, t1;
			float s2, t2;
			const vector4 *colour;
			const Material *material;
		};

		struct rcScreenshot_t {
			int width, height;
			GLuint pbo;
			GLsync sync;
			const char *name;
		};

		struct rcDrawModel_t {
			const Model *model;
		};

		struct RenderCommand {
		public:
			static void Init();
			static void Shutdown();

			enum class Type {
				DRAWQUAD = 0,
				DRAWMODEL,
				SCREENSHOT,
				NUM_RENDER_CMDS
			};
			union {
				rcDrawQuad_t drawQuad;
				rcScreenshot_t screenshot;
				rcDrawModel_t drawModel;
			};

			// don't allow default instantiation
			RenderCommand() = delete;

			RenderCommand( Type type )
			: type( type )
			{
			}

			void Execute( void ) const;

		private:
			Type type;
		};

	} // namespace Renderer

} // namespace XS
