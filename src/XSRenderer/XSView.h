#pragma once

namespace XS {

	namespace Renderer {

		class View {
		private:
		public:
			union {
				struct {
					// ...
				} data2d;
				struct {
					float matrix3DProjection[16];
					float matrix3DModelview[16];
				} data3d;
			};
			bool is2D;

			uint32_t width, height;
			std::vector<RenderCommand> renderCommands;

			void Register( void );
			void Bind( void );
			void PreRender( void ) const;
			void PostRender( void ) const;
		};

	} // namespace Renderer

} // namespace XS
