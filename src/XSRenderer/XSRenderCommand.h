#pragma once

#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Renderer {

		struct Material;
		class Model;

		//FIXME: polymorphism?
		enum class CommandType {
			DrawQuad,
			DrawModel,
			Screenshot
		};

		struct DrawQuadCommand {
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

		struct DrawModelCommand {
			const Model	*model;
		};

		struct ScreenshotCommand {
			int32_t		 width;
			int32_t		 height;
			GLuint		 pbo;
			GLsync		 sync;
			const char	*name;
		};

		struct RenderCommand {
		private:
			CommandType type;

		public:
			union {
				DrawQuadCommand		drawQuad;
				DrawModelCommand	drawModel;
				ScreenshotCommand	screenshot;
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

			RenderCommand( CommandType commandType )
			: type( commandType )
			{
			}

			// dispatch the render command to the associated function
			void Execute(
				void
			) const;
		};

	} // namespace Renderer

} // namespace XS
