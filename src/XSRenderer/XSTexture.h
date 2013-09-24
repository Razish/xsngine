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
			InternalFormat internalFormat;

		public:
			static void Init( void );

			Texture( unsigned int width, unsigned int height, InternalFormat internalFormat = RGBA8, byte *data = NULL );
			~Texture();
		};

	} // namespace Renderer

} // namespace XS
