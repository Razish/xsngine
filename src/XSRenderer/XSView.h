#pragma once

#include "XSCommon/XSMatrix.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSRenderCommand.h"

namespace XS {

	namespace Renderer {

		class Buffer;

		struct View {
			matrix4 projectionMatrix;
			matrix4 viewMatrix;

			Buffer *perFrameData;

			bool is2D;

			uint32_t width, height;
			std::vector<RenderCommand> renderCommands;

			View();
			~View();

			void Register( void );
			void Bind( void );
			void PreRender( void );
			void PostRender( void ) const;
		};

	} // namespace Renderer

} // namespace XS
