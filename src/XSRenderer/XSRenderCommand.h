#pragma once

namespace XS {

	namespace Renderer {

		enum renderCommandType_t {
			RC_DRAWQUAD=0,
			NUM_RCMDS
		};

		struct rcDrawQuad_t {
			float x, y;
			float w, h;
			float s1, t1;
			float s2, t2;
			uint32_t textureID;
		};

		struct RenderCommand {
			union {
				rcDrawQuad_t drawQuad;
			};
			renderCommandType_t type;

			void Execute( void );
		};

	} // namespace Renderer

} // namespace XS
