#pragma once

namespace XS {

	namespace Renderer {

		#define MAX_TEXTURES (256)

		typedef struct texture_s {
			unsigned int	id;
			unsigned int	width;
			unsigned int	height;
		} texture_t;

		void					Texture_Cleanup( void );
		texture_t *				Texture_Create( unsigned int width, unsigned int height, internalFormat_t internalFormat, unsigned int minFilter, unsigned int magFilter );
		texture_t *				Texture_CreateBlank( unsigned int width, unsigned int height, internalFormat_t internalFormat );
	}

}
