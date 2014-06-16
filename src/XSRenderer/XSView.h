#pragma once

#include "XSCommon/XSMatrix.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSRenderCommand.h"

namespace XS {

	namespace Renderer {

		class Buffer;

		struct View {
		private:
			uint32_t width, height;

		public:
			matrix4 projectionMatrix;
			matrix4 viewMatrix;
			Buffer *perFrameData;
			std::vector<RenderCommand> renderCommands;
			bool is2D;

			View( uint32_t width, uint32_t height, bool is2D );
			~View();

			void Bind( void );
			void PreRender( void );
			void PostRender( void ) const;
		};

	} // namespace Renderer

} // namespace XS
