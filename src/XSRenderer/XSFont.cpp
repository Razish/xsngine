#include "XSSystem/XSInclude.h"

#include "GLee/GLee.h"
#include "SDL2/SDL.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "freetype/ftglyph.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSFile.h"
#include "XSRenderer/XSFont.h"
#include "XSRenderer/XSShaderProgram.h"

namespace XS {
	
	namespace Renderer {

		static FT_Library ft;
		static std::unordered_map<const char *, font_s *> fonts;
		static ShaderProgram *fontProgram = NULL;

		font_s::font_s( const char *name, uint16_t size ) {
			this->file = String::Format( "fonts/%s.ttf", name );
			this->size = size;
		}

		void Font::Init( void ) {
			if ( FT_Init_FreeType( &ft ) )
				throw( "Could not initialise freetype library" );

			fonts["menu"] = new font_s( "menu", 48 );

			fontProgram = new ShaderProgram( "text", "text" );

			for ( auto it = fonts.begin(); it != fonts.end(); ++it ) {
				font_s *font = it->second;
				FT_Face face = NULL;
				
				File file( font->file.c_str(), File::READ );
				if ( file.length == 0 ) {
					Console::Print( "WARNING: Could not load font file '%s'\n", file.path );
					continue;
				}

				byte *contents = new byte[file.length];
				file.Read( contents );

				if ( FT_New_Memory_Face( ft, contents, file.length, 0, &face ) ) {
					Console::Print( "WARNING: Could not register font '%s'\n", font->file.c_str() );
					delete[] contents;
					continue;
				}
				delete[] contents;

				if ( FT_Set_Char_Size( face, font->size << 6, font->size << 6, 96, 96 ) ) {
					//TODO: appropriate warning message
					continue;
				}

				// load the printable characters
				for ( char c=0x20; c<0x7F; c++ ) {
					FT_Glyph glyph;
					
					uint32_t index = FT_Get_Char_Index( face, c );
					if ( !index )
						continue;

					// render the character's glyph
					FT_Load_Glyph( face, index, FT_LOAD_RENDER );
					FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );
					FT_Get_Glyph( face->glyph, &glyph );

					FT_GlyphSlot slot = face->glyph;
					FT_Bitmap& bitmap = slot->bitmap;

					// generate the texture
					uint32_t w = bitmap.width, h = bitmap.rows;
					uint32_t tex=0;
					glGenTextures( 1, &tex );
					glBindTexture( GL_TEXTURE_2D, tex );

					// allocate bitmap data
					byte *data = new byte[w*h];
					memset( data, 0, w*h*sizeof( byte ) );
					memcpy( data, bitmap.buffer, w*h*sizeof( byte ) );

					// set gl alignment to 1 byte, generate texture, restore alignment
					GLint alignment;
					glGetIntegerv( GL_UNPACK_ALIGNMENT, &alignment );
					glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
						glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, 0, GL_RED, w, h, GL_UNSIGNED_BYTE, data );
					glPixelStorei( GL_UNPACK_ALIGNMENT, alignment );

					delete[] data;
					FT_Done_Glyph( glyph );

					font->glyph[c].texture = tex;
					font->glyph[c].dimensions = vector4( (number)w, (number)h, (number)(slot->advance.x >> 6), (number)(slot->metrics.horiBearingY >> 6) );
				}

				FT_Done_Face( face );
			}
		}

		void Font::Draw( const vector2 pos, const std::string &text, const font_s *font ) {
			if ( text.empty() )
				return;
			//TODO: Font::Draw
		}

	} // namespace Renderer

} // namespace XS
