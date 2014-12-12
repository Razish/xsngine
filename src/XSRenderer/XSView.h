#pragma once

#include <vector>

#include "XSCommon/XSMatrix.h"
#include "XSRenderer/XSRenderCommand.h"

namespace XS {

	namespace Renderer {

		namespace Backend {
			class Buffer;
		} // namespace Backend

		class Renderable;

		struct View {
		private:
			uint32_t width, height;
			std::vector<const Renderable*> renderObjects;
			void (*callbackPreRender)( void );

		public:
			matrix4 projectionMatrix;
			matrix4 viewMatrix;
			Backend::Buffer *perFrameData;
			std::vector<RenderCommand> renderCommands;
			bool is2D;

			View( uint32_t width, uint32_t height, bool is2D );
			~View();

			void Bind( void );
			void PreRender( void );
			void PostRender( void ) const;

			void SetPreRenderCallback( void (*callback)( void ) );
			void AddObject( const Renderable *renderObject );
		};

	} // namespace Renderer

} // namespace XS
