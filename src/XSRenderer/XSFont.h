#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include "freetype/ftglyph.h"

#include "XSCommon/XSVector.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSMaterial.h"

namespace XS {

	namespace Renderer {

		struct fontData_t {
			vector2				size;
			FT_Glyph_Metrics	metrics;
		};

		struct font_t {
			font_t() : file( "" ), name(""), size( 24 ) {}
			font_t( const char *name, uint16_t size );
			void RenderGlyphs( void );

			std::string		file;
			std::string		name;
			uint16_t		size;
			Texture			*texture;
			Material		*material;
		};

		// should not be instantiated
		class Font {
		private:
			Font();

		public:
			static void Init( void );
			static void Shutdown( void );
			static font_t *Register( const char *name, uint16_t size );
			static void Draw( const vector2 pos, const std::string &text, const font_t *font );
		};

	} // namespace Renderer

} // namespace XS
