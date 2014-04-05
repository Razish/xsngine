#pragma once

namespace XS {

	namespace Renderer {

		struct rcDrawQuad_t {
			float x, y;
			float w, h;
			float s1, t1;
			float s2, t2;
			uint32_t textureID;
		};

		struct RenderCommand {
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
