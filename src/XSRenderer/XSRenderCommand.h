#pragma once

namespace XS {

	namespace Renderer {

		struct Material;

		struct rcDrawQuad_t {
			float x, y;
			float w, h;
			float s1, t1;
			float s2, t2;
			const Material *material;
		};

		struct rcScreenshot_t {
			int width, height;
			GLuint pbo;
			GLsync sync;
			const char *name;
		};

		struct RenderCommand {
		public:
			static void Init();
			static void Shutdown();

		public:
			enum RenderCommandType {
				DRAWQUAD = 0,
				SCREENSHOT,
				NUM_RCMDS
			};
			union {
				rcDrawQuad_t drawQuad;
				rcScreenshot_t screenshot;
			};

			RenderCommand( RenderCommandType type ) : type( type ) {}
			void Execute( void ) const;

		private:
			RenderCommand();
			RenderCommandType type;
		};

	} // namespace Renderer

} // namespace XS
