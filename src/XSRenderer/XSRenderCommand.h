#pragma once

namespace XS {

	namespace Renderer {

		enum CommandType {
			RC_DRAWQUAD=0,
			NUM_RCMDS
		};

		struct drawQuadStruct {
			float x, y;
			float w, h;
			float s1, t1;
			float s2, t2;
			uint32_t textureID;
		};

		class RenderCommand {
		private:
		public:
			union {
				drawQuadStruct drawQuad;
			};
			CommandType type;

			void Execute( void );
		};

	} // namespace Renderer

} // namespace XS
