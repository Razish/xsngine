#pragma once

#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Renderer {

		struct Material;
		class Model;

		struct rcDrawQuad_t {
			real32_t		 x;
			real32_t		 y;
			real32_t		 w;
			real32_t		 h;
			real32_t		 s1;
			real32_t		 t1;
			real32_t		 s2;
			real32_t		 t2;
			const vector4	*colour;
			const Material	*material;
		};

		struct rcScreenshot_t {
			int			 width;
			int			 height;
			GLuint		 pbo;
			GLsync		 sync;
			const char	*name;
		};

		struct rcDrawModel_t {
			const Model	*model;
		};

		struct RenderCommand {
		public:
			enum class Type {
				DRAWQUAD = 0,
				DRAWMODEL,
				SCREENSHOT,
				NUM_RENDER_CMDS
			};
			union {
				rcDrawQuad_t	drawQuad;
				rcScreenshot_t	screenshot;
				rcDrawModel_t	drawModel;
			};

			// don't allow default instantiation
			RenderCommand() = delete;

			// initialise render command queue
			// create default material, allocate GPU buffers
			static void Init(
				void
			);

			// clean up GPU resources
			static void Shutdown(
				void
			);

			RenderCommand( Type commandType )
			: type( commandType )
			{
			}

			// dispatch the render command to the associated function
			void Execute(
				void
			) const;

		private:
			Type type;
		};

	} // namespace Renderer

} // namespace XS
