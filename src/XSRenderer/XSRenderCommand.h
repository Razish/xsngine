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

		struct RenderCommand {
		public:
			static void Init();
			static void Shutdown();

		public:
			enum RenderCommandType {
				DRAWQUAD = 0,
				NUM_RCMDS
			};
			union {
				rcDrawQuad_t drawQuad;
			};

			RenderCommand( RenderCommandType type ) : type( type ) {}
			void Execute( void ) const;

		private:
			RenderCommand();
			RenderCommandType type;
		};

	} // namespace Renderer

} // namespace XS
