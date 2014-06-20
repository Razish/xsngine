#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSConsole.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSFont.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSVertexAttributes.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSImagePNG.h"

namespace XS {

	namespace Renderer {

		static FT_Library ft;
		static std::unordered_map<std::string, font_t *> fonts;
		static ShaderProgram *fontProgram = nullptr;


		font_t::font_t( const char *name, uint16_t size ) : name(name), size(size) {
			file = String::Format( "fonts/%s.ttf", name );
		}

		static Material *CreateFontMaterial( Texture& fontTexture ) {
			Material *fontMaterial = new Material();

			Material::SamplerBinding samplerBinding;
			samplerBinding.unit = 0;
			samplerBinding.texture = &fontTexture;

			fontMaterial->samplerBindings.push_back( samplerBinding );
			fontMaterial->shaderProgram = fontProgram;

			return fontMaterial;
		}

		void font_t::RenderGlyphs( void ) {
			FT_Face face = nullptr;

			const File ttf( file.c_str(), FileMode::READ_BINARY );
			if ( !ttf.open ) {
				Console::Print( "WARNING: Could not load font file \"%s\"\n", ttf.path );
				return;
			}

			byte *contents = new byte[ttf.length];
			std::memset( contents, 0, ttf.length );
			ttf.Read( contents );

			if ( FT_New_Memory_Face( ft, contents, ttf.length, 0, &face ) ) {
				Console::Print( "WARNING: Could not register font \"%s\"\n", file.c_str() );
				delete[] contents;
				return;
			}

			const unsigned int dpi = 96;
			if ( FT_Set_Char_Size( face, size << 6, size << 6, dpi, dpi ) ) {
				//TODO: appropriate warning message
				delete[] contents;
				return;
			}

			const size_t numChars = 0xFFu;
			byte *atlas = new byte[numChars * size * size * 4];
			std::memset( atlas, 0, numChars * size * size * 4 );

			fontData_t fontData[numChars], *currentFD = fontData;
			std::memset( fontData, 0, sizeof(fontData) );

			// load the printable characters
			for ( char c = 0x20;
				c < numChars;
				c++, currentFD++ )
			{
				uint32_t index = FT_Get_Char_Index( face, c );
				if ( !index ) {
					continue;
				}

				// render the character's glyph
				FT_Glyph glyph;
				FT_Load_Glyph( face, index, FT_LOAD_RENDER );
				FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO );
				FT_Get_Glyph( face->glyph, &glyph );

				FT_GlyphSlot slot = face->glyph;
				FT_Bitmap& bitmap = slot->bitmap;
				const int width = bitmap.width;
				const int height = bitmap.rows;

				// generate an RGBA texture from the 8bpp glyph
				// atlas will be 16 chars by 16 chars
				const size_t rowSize = size * 16 * 4;
				const size_t offset = (c * size * size * 4);
				for ( size_t row = 0; row < height; row++ ) {
					for ( size_t col = 0; col < width; col++ ) {
						for ( size_t channel = 0; channel < 4; channel++ ) {
							atlas[offset + (col * 4) + (row * rowSize) + channel] = bitmap.buffer[(row * (rowSize * (int)(c / 16))) + col];
						}
					}
				}
				currentFD->size = vector2( width, height );
				std::memcpy( &currentFD->metrics, &slot->metrics, sizeof(FT_Glyph_Metrics) );

				FT_Done_Glyph( glyph );
			}

			FT_Done_Face( face );
			delete[] contents;

			// save out the glyph positioning data
			const File fontdat( String::Format( "cache/fonts/%s.fontdat", name.c_str() ).c_str(), FileMode::WRITE_BINARY );
			fontdat.Write( &fontData, sizeof(fontData) );

			texture = new Texture( size * 16, size * 16, InternalFormat::RGBA8, atlas );
			SDL_assert( texture );
			material = CreateFontMaterial( *texture );
			SDL_assert( material );

			delete[] atlas;
		}

		void Font::Init( void ) {
			if ( FT_Init_FreeType( &ft ) ) {
				throw( XSError( "Could not initialise freetype library" ) );
			}

			fonts["menu"] = new font_t( "menu", 48 );

			static const VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_TexCoord" },
				{ 2, "in_Color" }
			};

			fontProgram = new ShaderProgram( "text", "text", attributes, sizeof(attributes) / sizeof(attributes[0]) );

			for ( const auto &it: fonts ) {
				it.second->RenderGlyphs();
			}
		}

		void Font::Shutdown( void ) {
			for ( const auto &it : fonts ) {
				delete it.second;
			}

			delete fontProgram;
		}

		font_t *Font::Register( const char *name, uint16_t size ) {
			font_t *font = fonts[name];
			if ( font ) {
				if ( font->size == size ) {
					return font;
				}
				else {
					// size mismatch, we need to regenerate it
					delete font;
				}
			}

			font = fonts[name] = new font_t( name, size );
			font->RenderGlyphs();
			return font;
		}

		void Font::Draw( const vector2 pos, const std::string &text, const font_t *font ) {
			SDL_assert( font && "Tried to call Font::Draw with an unregistered font" );

			if ( text.empty() ) {
				return;
			}

		#if 0
			vector2 currentPos = pos;
			for ( size_t i = 0; i < text.length(); i++ ) {
				const char c = text[i];
				const vector4 *v = &font->glyph[c].dimensions;
				DrawQuad( currentPos.x + v->z, currentPos.y + v->w, // x, y
					v->x, v->y, // width, height
					0.0f, 0.0f, 1.0f, 1.0f, // st coords
					*font->glyph[c].material );
				currentPos.x += v->x;
				if ( c == '\n' ) {
					currentPos.y += v->y;
				}
			}
		#endif
		//	DrawQuad( pos.x, pos.y, font->size * 16, font->size * 16, 0.0f, 0.0f, 1.0f, 1.0f, *font->material );
		}

	} // namespace Renderer

} // namespace XS
