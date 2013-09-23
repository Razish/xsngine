#pragma once

namespace XS {

	namespace Renderer {

		#define MAX_TEXTURES (256)

		class Texture {
		private:
			Texture(){}

		public:

			uint32_t id;
			unsigned int width, height;

			static void Init( void );
			static void	Cleanup( void );

			Texture( unsigned int width, unsigned int height, internalFormat_t internalFormat, unsigned int minFilter, unsigned int magFilter );
			Texture( unsigned int width, unsigned int height, internalFormat_t internalFormat );
		};

	}

}
