#pragma once

namespace XS {

	namespace Renderer {

		#define MAX_TEXTURES (256)

		class Texture {
			friend class Framebuffer;
		private:
			Texture(){}

			uint32_t id;
			unsigned int width, height;

		public:
			static void Init( void );

			Texture( unsigned int width, unsigned int height, internalFormat_t internalFormat );
			~Texture();
		};

	}

}
