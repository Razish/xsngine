#include <ft2build.h>
#include FT_FREETYPE_H
#include "freetype/ftglyph.h"

#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSConsole.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSFont.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSVertexAttributes.h"

namespace XS {

	namespace Renderer {

		static FT_Library ft;
		static std::unordered_map<const char *, font_t *> fonts;
		static ShaderProgram *fontProgram = nullptr;

		font_t::font_t( const char *name, uint16_t size ) {
			this->file = String::Format( "fonts/%s.ttf", name );
			this->size = size;
		}

		void Font::Init( void ) {
			if ( FT_Init_FreeType( &ft ) ) {
				throw( XSError( "Could not initialise freetype library" ) );
			}

			fonts["menu"] = new font_t( "menu", 48 );

			static const Renderer::VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_TexCoord" },
				{ 2, "in_Color" }
			};

			fontProgram = new ShaderProgram( "text", "text", attributes, sizeof(attributes) / sizeof(attributes[0]) );

			for ( const auto &it : fonts ) {
				font_t *font = it.second;
				FT_Face face = nullptr;

				const File f( font->file.c_str(), FileMode::READ_BINARY );
				if ( !f.open ) {
					Console::Print( "WARNING: Could not load font file \"%s\"\n", f.path );
					continue;
				}

				byte *contents = new byte[f.length];
				memset( contents, 0, f.length );
				f.Read( contents );

				if ( FT_New_Memory_Face( ft, contents, f.length, 0, &face ) ) {
					Console::Print( "WARNING: Could not register font \"%s\"\n", font->file.c_str() );
					delete[] contents;
					continue;
				}

				if ( FT_Set_Char_Size( face, font->size << 6, font->size << 6, 96, 96 ) ) {
					//TODO: appropriate warning message
					delete[] contents;
					continue;
				}

				// load the printable characters
				for ( char c=0x20; c<0x7F; c++ ) {
					FT_Glyph glyph;

					uint32_t index = FT_Get_Char_Index( face, c );
					if ( !index ) {
						continue;
					}

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
						glTexImage2D( GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data );
					glPixelStorei( GL_UNPACK_ALIGNMENT, alignment );

					delete[] data;
					FT_Done_Glyph( glyph );

					font->glyph[c].texture = tex;
					font->glyph[c].dimensions = vector4( (number)w, (number)h, (number)(slot->advance.x >> 6),
						(number)(slot->metrics.horiBearingY >> 6) );
				}

				FT_Done_Face( face );
				delete[] contents;
			}
		}

		void Font::Shutdown( void ) {
			for ( const auto &it : fonts ) {
				delete it.second;
			}

			delete fontProgram;
		}

		void Font::Draw( const vector2 pos, const std::string &text, const font_t *font ) {
			if ( text.empty() ) {
				return;
			}
			//TODO: Font::Draw
		}

	} // namespace Renderer

} // namespace XS
