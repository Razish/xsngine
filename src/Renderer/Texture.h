#pragma once

#include "Renderer/InternalFormat.h"

namespace Renderer {

	const uint32_t maxTextures = 256u;
	const uint32_t maxTextureUnits = 16u;

	class Texture {

	public:
		using ID = unsigned int; // GLuint

	private:
		static const Texture	*lastUsedTexture[maxTextureUnits];
		static int				 lastUsedTextureUnit;

	public:
		ID	id;

		// don't allow default instantiation
		Texture() = delete;
		Texture( const Texture& ) = delete;
		Texture& operator=( const Texture& ) = delete;

		static void Init(
			void
		);

		static void Shutdown(
			void
		);

		// create a texture
		Texture(
			uint32_t width,
			uint32_t height,
			InternalFormat internalFormat = InternalFormat::RGBA8,
			const uint8_t *data = nullptr
		);

		~Texture();

		// bind the current texture to the specified unit for subsequent rendering
		void Bind(
			int unit
		) const;

	};

} // namespace Renderer
