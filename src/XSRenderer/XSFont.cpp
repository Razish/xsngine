#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSError.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSFont.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSVertexAttributes.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSTexture.h"

namespace XS {

	namespace Renderer {

		static FT_Library ft;
		static std::unordered_map<std::string, Font *> fonts;
		static ShaderProgram *fontProgram = nullptr;

		Font::Font( const char *name, uint16_t size )
		: name( name ), size( size )
		{
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

			uint8_t *contents = new uint8_t[ttf.length];
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
			const size_t skip = 0x20u;
			const size_t atlasSize = numChars * size * size;
			uint8_t *atlas = new uint8_t[atlasSize];
			std::memset( atlas, 0, atlasSize );

			if ( Common::com_developer->GetBool() ) {
				console.Print( "Generating font atlas for '%s' (%ix%i)\n", name.c_str(), size * 16, size * 16 );
			}

			lineHeight = (float)face->size->metrics.height / 64.0f;

			// load the printable characters
			for ( char c = skip; c < numChars; c++ ) {
				const uint32_t index = FT_Get_Char_Index( face, c );
				if ( !index ) {
					continue;
				}

				// render the character's glyph
				FT_Load_Glyph( face, index, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT );
				if ( FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ) ) {
					continue;
				}
				FT_Glyph glyph;
				FT_Get_Glyph( face->glyph, &glyph );

				FT_GlyphSlot slot = face->glyph;
				FT_Bitmap& bitmap = slot->bitmap;
				const int width = bitmap.width ? bitmap.width : static_cast<float>( slot->advance.x ) / 64.0f;
				const int height = bitmap.rows;

				// atlas will be 16 chars by 16 chars, where chars are WxH but spaced by size*size
				const int col = c % 16, row = c / 16;
				const size_t rowSize = size * 16;
				const size_t x = col * size; // horizontal position
				const size_t y = row * size * rowSize; // vertical position

				// calculate glyph metrics
				FontData &fd = data[c];
				fd.size = vector2( width, height );
				const float colPos = static_cast<float>( col ) / 16.0f, rowPos = static_cast<float>( row ) / 16.0f;
				fd.s = vector2( colPos, colPos + (static_cast<float>( width ) / 256.0f) );
				fd.t = vector2( rowPos, rowPos + (static_cast<float>( height ) / 256.0f) );
				fd.advance = static_cast<float>( slot->advance.x ) / 64.0f;
				fd.offset.x = static_cast<float>( slot->metrics.horiBearingX ) / 64.0f;
				fd.offset.y = lineHeight + -(static_cast<float>( slot->metrics.horiBearingY ) / 64.0f);

				if ( !bitmap.buffer /*|| !width || !height*/ ) {
					FT_Done_Glyph( glyph );
					continue;
				}

				// generate an RGBA texture from the 8bpp glyph
				const size_t topLeft = y + x; // row + column
			//	const size_t topRight = y + x + size; // (row + column) + charWidth
			//	const size_t bottomLeft = y + x + (size * rowSize);
			//	const size_t bottomRight = y + x + (size * rowSize) + size;

				for ( size_t y = 0; y < height; y++ ) {
					for ( size_t x = 0; x < width; x++ ) {
						atlas[topLeft + (y * rowSize) + x] = bitmap.buffer[y * width + x];
					}
				}

			//	WritePNG( String::Format( "cache/fonts/%s_%i_%c.png", name.c_str(), size, c ).c_str(), bitmap.buffer,
			//		width, height, 1 );
				FT_Done_Glyph( glyph );
			}

			FT_Done_Face( face );
			delete[] contents;

			// save out the font atlas
			WritePNG( String::Format( "cache/fonts/%s_%i.png", name.c_str(), size ).c_str(), atlas, size * 16,
				size * 16, 1 );

			// and now the glyph metrics
			const File fontdat( String::Format( "cache/fonts/%s_%i.fontdat", name.c_str(), size ).c_str(),
				FileMode::WRITE_BINARY );
			fontdat.Write( &data[skip], sizeof(data) - (sizeof(*data) * skip) );

			texture = new Texture( size * 16, size * 16, InternalFormat::R8, atlas );
			SDL_assert( texture );
			material = CreateFontMaterial( *texture );
			SDL_assert( material );

			delete[] atlas;
		}

		uint32_t Font::Draw( const vector2 &pos, const std::string &text ) {
			uint32_t numLines = 0u;
			if ( text.empty() ) {
				return numLines;
			}

			vector2 currentPos = pos;
			for ( size_t i = 0; i < text.length(); i++ ) {
				const char c = text[i];
				const FontData &fd = data[c];

				// check for overflow
				if ( currentPos.x + fd.advance >= vid_width->GetInt() ) {
					currentPos.x = pos.x;
					currentPos.y += lineHeight;
					numLines++;
				}

				DrawQuad( currentPos.x + fd.offset.x, currentPos.y + fd.offset.y, // x, y
					fd.size.x, fd.size.y, // width, height
					fd.s.x, fd.t.x, fd.s.y, fd.t.y, // st coords
					nullptr, material );
				currentPos.x += fd.advance;
				if ( c == '\n' ) {
					currentPos.x = pos.x;
					currentPos.y += lineHeight;
					numLines++;
				}
			}

			return numLines;
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

			fontProgram = new ShaderProgram( "text", "text", attributes, ARRAY_LEN( attributes ) );

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

			if ( Common::com_developer->GetBool() ) {
				console.Print( "Generating new font '%s' at size %i\n", name, size );
			}
			font = fonts[name] = new Font( name, size );
			font->RenderGlyphs();
			return font;
		}

	} // namespace Renderer

} // namespace XS
