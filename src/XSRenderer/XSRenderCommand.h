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
			enum RenderCommandType {
				DRAWQUAD = 0,
				NUM_RCMDS
			};
			union {
				rcDrawQuad_t drawQuad;
			};
			RenderCommandType type;

			void Execute( void ) const;
		};

	} // namespace Renderer

} // namespace XS
