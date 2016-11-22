#pragma once

#include "XSRenderer/XSTexture.h"

namespace Renderer {

	#define MAX_FBO_COLOR_TEXTURES (4)

	class Framebuffer {

	public:
		static const Framebuffer	*currentReadFramebuffer;
		static const Framebuffer	*currentWriteFramebuffer;

		uint32_t		 id;
		const Texture	*colourTextures[MAX_FBO_COLOR_TEXTURES];
		const Texture	*depthTexture;
		unsigned int	 stencilTexture;

		// bind the default framebuffer
		static void BindDefault(
			void
		);

		// retrieve the currently bound framebuffer
		static const Framebuffer *GetCurrent(
			void
		) XS_WARN_UNUSED_RESULT;

		// blit from source framebuffer to destination framebuffer
		static void Blit(
			const Framebuffer *source,
			const Framebuffer *destination,
			int sourceWidth,
			int sourceHeight,
			int destWidth,
			int destHeight,
			uint32_t bufferBits
		);

		// blit colour from source framebuffer to destination framebuffer
		static void BlitColour(
			const Framebuffer *source,
			const Framebuffer *destination,
			int sourceWidth,
			int sourceHeight,
			int destWidth,
			int destHeight
		);

		// blit colour and depth from source framebuffer to destination framebuffer
		static void BlitColourAndDepth(
			const Framebuffer *source,
			const Framebuffer *destination,
			int sourceWidth,
			int sourceHeight,
			int destWidth,
			int destHeight
		);

		// create a framebuffer
		Framebuffer();

		// clean up a framebuffer
		~Framebuffer();

		// attach a colour texture to the framebuffer object
		void AttachColourTexture(
			const Texture *texture,
			unsigned int slot
		);

		// attach a depth texture to the framebuffer object
		void AttachDepthTexture(
			const Texture *texture
		);

		// attach a stencil texture to the framebuffer object
		void AttachDepthStencilTexture(
			const Texture *texture
		);

		// bind the framebuffer for subsequent rendering
		void Bind(
			void
		) const;

		// check if framebuffer usage is valid
		bool Check(
			void
		) const;

	};

} // namespace Renderer
