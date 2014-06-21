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
		static std::unordered_map<std::string, Font *> fonts;
		static ShaderProgram *fontProgram = nullptr;


		Font::Font( const char *name, uint16_t size ) : name(name), size(size) {
			file = String::Format( "fonts/%s.ttf", name );
			std::memset( data, 0, sizeof(data) );
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

		void Font::RenderGlyphs( void ) {
			FT_Face face = nullptr;

			const File ttf( file.c_str(), FileMode::READ_BINARY );
			if ( !ttf.open ) {
				console.Print( "WARNING: Could not load font file \"%s\"\n", ttf.path );
				return;
			}

			byte *contents = new byte[ttf.length];
			std::memset( contents, 0, ttf.length );
			ttf.Read( contents );

			if ( FT_New_Memory_Face( ft, contents, ttf.length, 0, &face ) ) {
				console.Print( "WARNING: Could not register font \"%s\"\n", file.c_str() );
				delete[] contents;
				return;
			}

			const unsigned int dpi = 96;
			if ( FT_Set_Char_Size( face, size << 6, size << 6, dpi, dpi ) ) {
				//TODO: appropriate warning message
				delete[] contents;
				return;
			}

			const size_t numChars = 256u;
			byte *atlas = new byte[numChars * size * size];
			std::memset( atlas, 0, numChars * size * size );

			// load the printable characters
			for ( char c = 0x20; c < numChars; c++ ) {
				uint32_t index = FT_Get_Char_Index( face, c );
				if ( !index ) {
					continue;
				}

				// render the character's glyph
				FT_Load_Glyph( face, index, FT_LOAD_RENDER );
				if ( FT_Render_Glyph( face->glyph, FT_RENDER_MODE_MONO ) ) {
					continue;
				}
				FT_Glyph glyph;
				FT_Get_Glyph( face->glyph, &glyph );

				FT_GlyphSlot slot = face->glyph;
				FT_Bitmap& bitmap = slot->bitmap;
				const int width = bitmap.width;
				const int height = bitmap.rows;

				// generate an RGBA texture from the 8bpp glyph
				// atlas will be 16 chars by 16 chars
				const size_t rowSize = size * 16;
				const size_t topleft = ((c / 16) * size) * rowSize + (c % 16) * size;
				for ( size_t y = 0; y < height; y++ ) {
					for ( size_t x = 0; x < width; x++ ) {
						atlas[topleft + (y * rowSize) + x] = bitmap.buffer[y * width + x];
					}
				}
				data[c].size = vector2( width, height );
				std::memcpy( &data[c].metrics, &slot->metrics, sizeof(FT_Glyph_Metrics) );

				FT_Done_Glyph( glyph );
			}

			FT_Done_Face( face );
			delete[] contents;

			// save out the glyph atlas with positioning data
			WritePNG( String::Format( "cache/fonts/%s_%i.png", name.c_str(), size ).c_str(), atlas,
				size * 16, size * 16, 1 );

			const size_t skip = 0x20u;
			const File fontdat( String::Format( "cache/fonts/%s_%i.fontdat", name.c_str(), size ).c_str(),
				FileMode::WRITE_BINARY );
			fontdat.Write( &data[skip], sizeof(data) - (sizeof(*data) * skip) );

			texture = new Texture( size * 16, size * 16, InternalFormat::R8, atlas );
			SDL_assert( texture );
			material = CreateFontMaterial( *texture );
			SDL_assert( material );

			delete[] atlas;
		}

		void Font::Draw( const vector2 &pos, const std::string &text ) {
			if ( text.empty() ) {
				return;
			}

		#if 0
			vector2 currentPos = pos;
			for ( size_t i = 0; i < text.length(); i++ ) {
				const char c = text[i];
				const vector4 *v = &glyph[c].dimensions;
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
			DrawQuad( pos.x, pos.y, size * 16, size * 16, 0.0f, 0.0f, 1.0f, 1.0f, *material );
		}

		void Font::Init( void ) {
			if ( FT_Init_FreeType( &ft ) ) {
				throw( XSError( "Could not initialise freetype library" ) );
			}

			fonts["menu"] = new Font( "menu", 24 );
			fonts["console"] = new Font( "console", 12 );

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

		Font *Font::Register( const char *name, uint16_t size ) {
			Font *font = fonts[name];
			if ( font ) {
				if ( font->size == size ) {
					return font;
				}
				else {
					// size mismatch, we need to regenerate it
					delete font;
				}
			}

			font = fonts[name] = new Font( name, size );
			font->RenderGlyphs();
			return font;
		}

	} // namespace Renderer

} // namespace XS
