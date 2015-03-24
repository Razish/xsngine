#pragma once

#include <vector>
#include <queue>

#include "XSCommon/XSMatrix.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSBuffer.h"

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

namespace XS {

	namespace Renderer {

		// forward declarations
		class Renderable;

		// used for pre/post render callbacks
		typedef void (*renderCallback_t)( real64_t dt );

		struct View {
		private:
			uint32_t						width, height;
			std::vector<const Renderable*>	renderObjects;
			renderCallback_t				callbackPreRender, callbackPostRender;

		public:
			glm::mat4					 projectionMatrix;
			Backend::Buffer				*perFrameData;
			std::queue<RenderCommand>	 renderCommands;
			bool						 is2D;

			// construct a view, specifying additional callbacks if necessary
			// if width or height are 0, they are both inherited from the current resolution
			View(
				uint32_t viewWidth,
				uint32_t viewHeight,
				bool is2D,
				renderCallback_t preRender = nullptr,
				renderCallback_t postRender = nullptr
			);

			// do not call publicly
			~View() {
				delete perFrameData;
			}

			// bind a view so that subsequent render calls are associated with it
			void Bind(
				void
			);

			// prepare the view for rendering, i.e. adding objects to the scene
			// do not call externally, set callbacks via constructor
			void PreRender(
				real64_t dt
			);

			// post-process step if necessary
			// do not call externally, set callbacks via constructor
			void PostRender(
				real64_t dt
			);

			// add a renderable object to the view for this frame
			void AddObject(
				const Renderable *renderObject
			);
		};

	} // namespace Renderer

} // namespace XS
