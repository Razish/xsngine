#pragma once

namespace XS {

	namespace Renderer {

		uint8_t *LoadPNG( const char *filename, uint32_t *outWidth = nullptr, uint32_t *outHeight = nullptr );
		bool WritePNG( const char *filename, uint8_t *pixels, int w, int h, int numChannels = 4 );

	} // namespace Renderer

} // namespace XS
