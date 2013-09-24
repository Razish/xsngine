#pragma once

namespace XS {

	namespace Renderer {

		#define MAX_FBO_COLOR_TEXTURES (4)

		class Framebuffer {
		private:
			uint32_t		id;
			const Texture	*colorTextures[MAX_FBO_COLOR_TEXTURES];
			const Texture	*depthTexture;
			unsigned int	stencilTexture;
		
		public:
			static const Framebuffer *currentReadFramebuffer;
			static const Framebuffer *currentWriteFramebuffer;

			static void Init( void );
			static void Cleanup( void );
			static void BindDefault( void );
			static const Framebuffer *GetCurrent( void );
			static void Blit( const Framebuffer *source, const Framebuffer *destination, int sourceWidth, int sourceHeight, int destWidth, int destHeight, unsigned int bufferBits );
			static void BlitColor( const Framebuffer *source, const Framebuffer *destination, int sourceWidth, int sourceHeight, int destWidth, int destHeight );
			static void BlitColorAndDepth( const Framebuffer *source, const Framebuffer *destination, int sourceWidth, int sourceHeight, int destWidth, int destHeight );

			Framebuffer();
			~Framebuffer();
			void AttachColorTexture( const Texture *texture, unsigned int slot );
			void AttachDepthTexture( const Texture *texture );
			void AttachDepthStencilTexture( const Texture *texture );
			void AttachDepthRenderbuffer( unsigned int renderbufferId );
			void AttachStencilRenderbuffer( unsigned int renderbufferId );
			void Bind( void );
			void Check( void );
		};

	} // namespace Renderer

} // namespace XS
