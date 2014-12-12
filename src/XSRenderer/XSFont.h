#pragma once

#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
#include "freetype/ftglyph.h"

#include "XSCommon/XSVector.h"

namespace XS {

	namespace Renderer {

		class Texture;
		class Material;

		struct FontData {
			vector2	size;
			vector2	s, t;
			vector2	offset;
			float	advance;
		};

		class Font {
		private:
			Texture		*texture;
			Material	*material;

		public:
			std::string	file;
			std::string	name;
			uint16_t	size;
			FontData	data[256];
			float		lineHeight;

			Font( const char *name, uint16_t size );

			// don't allow default instantiation
			Font() = delete;
			Font( const Font& ) = delete;
			Font& operator=( const Font& ) = delete;

			void	RenderGlyphs	( void );
			void	Draw			( const vector2 &pos, const std::string &text );

			static void		Init			( void );
			static void		Shutdown		( void );
			static Font		*Register		( const char *name, uint16_t size );
		};

	} // namespace Renderer

} // namespace XS
