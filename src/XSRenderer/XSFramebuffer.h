#pragma once

namespace XS {

	namespace Renderer {

		#define MAX_FBO_COLOR_TEXTURES (4)

		typedef struct framebuffer_s {
			unsigned int	id;
			const texture_t	*colorTextures[MAX_FBO_COLOR_TEXTURES];
			const texture_t	*depthTexture;
			unsigned int	stencilTexture;
		} framebuffer_t;

		void					Framebuffer_Init( void );
		void					Framebuffer_Cleanup( void );
		framebuffer_t *			Framebuffer_Create( void );
		void					Framebuffer_AttachColorTexture( framebuffer_t *framebuffer, const texture_t *texture, unsigned int slot );
		void					Framebuffer_AttachDepthTexture( framebuffer_t *framebuffer, const texture_t *texture );
		void					Framebuffer_AttachDepthStencilTexture( framebuffer_t *framebuffer, const texture_t *texture );
		void					Framebuffer_AttachDepthRenderbuffer( framebuffer_t *framebuffer, unsigned int renderbufferId );
		void					Framebuffer_AttachStencilRenderbuffer( framebuffer_t *framebuffer, unsigned int renderbufferId );
		void					Framebuffer_Check( const framebuffer_t *framebuffer );

		void					Framebuffer_BindDefault( void );
		void					Framebuffer_Bind( const framebuffer_t *framebuffer );
		const framebuffer_t *	Framebuffer_GetCurrent( void );
		void					Framebuffer_Blit( const framebuffer_t *source, const framebuffer_t *destination, int sourceWidth, int sourceHeight, int destWidth, int destHeight, unsigned int bufferBits );
		void					Framebuffer_BlitColor( const framebuffer_t *source, const framebuffer_t *destination, int sourceWidth, int sourceHeight, int destWidth, int destHeight );
		void					Framebuffer_BlitColorAndDepth( const framebuffer_t *source, const framebuffer_t *destination, int sourceWidth, int sourceHeight, int destWidth, int destHeight );

		unsigned int			CreateRenderbuffer( unsigned int width, unsigned int height, internalFormat_t internalFormat );
	} // namespace Renderer

} // namespace XS
