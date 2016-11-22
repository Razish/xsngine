#pragma once

#include <string>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H
#include "freetype/ftglyph.h"

#include "Common/Vector.h"

namespace Renderer {

	class Texture;
	struct Material;

	struct FontData {

		vector2		pixelSize;
		vector2		s, t;
		vector2		offset;
		real32_t	advance;

	};

	//
	// Font overview
	// a Font object provides mechanisms for rendering ascii text at different sizes
	// Fonts are rendered and cached to disk on first load for faster subsequent loads. if an artist wants to
	//	prototype different fonts, simply changing the source assets will cause the cache to be regenerated
	//
	// performance notes
	// all functions may cause the glyphs to be rendered if cache is not valid
	// don't construct multiple Font objects for different sizes, a single Font can render at multiple sizes
	//
	// implementation details
	// all textures, materials and metrics are stored in the single Font using the point size as the key
	//

	class Font {

	private:
		std::map<uint16_t, Texture*>	texture;
		std::map<uint16_t, Material*>	material;

	public:
		// don't allow default instantiation
		Font() = delete;
		Font( const Font& ) = delete;
		Font& operator=( const Font& ) = delete;

		std::string						name;
		std::string						file;
		std::map<uint16_t, bool>		rendered;
		std::map<uint16_t, FontData>	data[256];
		std::map<uint16_t, real32_t>	lineHeight;

		// register a new font
		Font(
			const char *name
		);

		// generate a glyph atlas for the font at the current size.
		void RenderGlyphs(
			uint16_t pointSize
		);

		// draw a string at the given position
		void Draw(
			const vector2 &pos,
			const std::string &text,
			uint16_t pointSize,
			const vector4 *colour = nullptr
		);

		// return the number of lines it would take to draw (for linefeeds and wrapping)
		uint32_t GetTextLineCount(
			const vector2 &pos,
			const std::string &text,
			uint16_t pointSize
		) XS_WARN_UNUSED_RESULT;

		// return the pixel width of the specified character
		real32_t GetGlyphWidth(
			char c,
			uint16_t pointSize
		);

		// initialise the font system (i.e. freetype)
		static void Init(
			void
		);

		// free all font resources
		static void Shutdown(
			void
		);

		// register a new font, or be returned an existing one
		static Font *Register(
			const char *name
		);

	};

} // namespace Renderer
