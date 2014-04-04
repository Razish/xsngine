#pragma once

namespace XS {

	namespace Renderer {

		#define MAX_TEXTURES (256)

		class Texture {
		private:
			Texture();

			unsigned int width, height;
			InternalFormat internalFormat;

		public:
			static void Init( void );

			Texture( unsigned int width, unsigned int height, InternalFormat internalFormat = InternalFormat::RGBA8,
				byte *data = NULL );
			~Texture();

			uint32_t id;
		};

	} // namespace Renderer

} // namespace XS
