#pragma once

#include "XSRenderer/XSInternalFormat.h"

namespace XS {

	namespace Renderer {

		#define MAX_TEXTURES (256)
		const int MAX_TEXTURE_UNITS = 16;

		class Texture {
		private:
			static const Texture *lastUsedTexture[MAX_TEXTURE_UNITS];
			static int lastUsedTextureUnit;

			Texture();

			unsigned int width, height;
			InternalFormat internalFormat;

		public:
			static void Init( void );

			Texture( unsigned int width, unsigned int height, InternalFormat internalFormat = InternalFormat::RGBA8,
				byte *data = nullptr );
			~Texture();

			void Bind( int unit ) const;

			uint32_t id;
		};

	} // namespace Renderer

} // namespace XS
