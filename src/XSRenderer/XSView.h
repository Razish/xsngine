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
		class Framebuffer;
		class Texture;

		// used for pre/post render callbacks
		typedef void (*RenderCallback)( real64_t dt );

		struct View {
			enum RenderTarget {
				Diffuse,
				Normal,
				Position,
				TexCoord,
				Lighting,
				Depth,
				NumRenderTargets
			};
		private:
			Texture							*textures[NumRenderTargets];
			std::queue<const Renderable*>	 renderObjects;
			std::queue<vector3>				 pointLights;
			RenderCallback					 callbackPreRender;
			RenderCallback					 callbackPostRender;

		public:
			bool						 is2D;
			uint32_t					 width;
			uint32_t					 height;
			Framebuffer					*fbo;

			Backend::Buffer				*perFrameData;
			glm::mat4					 projectionMatrix;
			glm::mat4					 worldMatrix;

			std::queue<RenderCommand>	 renderCommands;

			// construct a view, specifying additional callbacks if necessary
			// if width or height are 0, they are both inherited from the current resolution
			View(
				uint32_t viewWidth,
				uint32_t viewHeight,
				bool is2D,
				RenderCallback preRender = nullptr,
				RenderCallback postRender = nullptr
			);

			// do not call publicly
			~View();

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

			// add a point light to the view for this frame
			void AddPointLight(
				const vector3 &lightPos
			);
		};

	} // namespace Renderer

} // namespace XS
