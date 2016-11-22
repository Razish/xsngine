#pragma once

#include <vector>
#include <queue>

#include "Common/Matrix.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Buffer.h"

#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
	#pragma GCC diagnostic pop
#endif

namespace Renderer {

	// forward declarations
	struct RenderInfo;
	class Framebuffer;
	class Texture;

	// used for pre/post render callbacks
	using RenderCallback = void (*)( real64_t dt );

	struct View {

	private:
		Texture					*colourTexture;
		Texture					*depthTexture;
		std::queue<RenderInfo>	 renderObjects;
		std::queue<vector3>		 pointLights;
		RenderCallback			 callbackPreRender;
		RenderCallback			 callbackPostRender;

	public:
		bool						 is2D;
		uint32_t					 width;
		uint32_t					 height;
		Framebuffer					*fbo;
		Backend::Buffer				*perFrameData;
		glm::mat4					 projectionMatrix;
		std::queue<RenderCommand>	 renderCommands;

		// construct a view, specifying additional callbacks if necessary
		// if width or height are 0, they are both inherited from the window's resolution
		View(
			uint32_t viewWidth,
			uint32_t viewHeight,
			bool is2D,
			RenderCallback preRender = nullptr,
			RenderCallback postRender = nullptr
		);
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
			const RenderInfo &renderInfo
		);

		// add a point light to the view for this frame
		void AddPointLight(
			const vector3 &lightPos
		);

	};

} // namespace Renderer
