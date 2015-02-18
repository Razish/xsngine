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

		static FT_Library freetypeLib;
		static std::unordered_map<std::string, Font *> fonts;
		static ShaderProgram *fontProgram = nullptr;
		static const uint32_t charsPerLine = 16u;

		Font::Font( const char *name, uint16_t pointSize )
		: name( name ), pointSize( pointSize )
		{
			file = String::Format( "fonts/%s.ttf", name );
			std::memset( data, 0u, sizeof(data) );
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

			const File ttf( file.c_str(), FileMode::ReadBinary );
			if ( !ttf.open ) {
				console.Print( PrintLevel::Normal, "WARNING: Could not load font file \"%s\"\n", ttf.path );
				return;
			}

			uint8_t *contents = new uint8_t[ttf.length];
			std::memset( contents, 0, ttf.length );
			ttf.Read( contents );

			if ( FT_New_Memory_Face( freetypeLib, contents, ttf.length, 0, &face ) ) {
				console.Print( PrintLevel::Normal, "WARNING: Could not register font \"%s\"\n", file.c_str() );
				delete[] contents;
				return;
			}

			const uint32_t dpi = 96u;
			if ( FT_Set_Char_Size( face, pointSize << 6, pointSize << 6, dpi, dpi ) ) {
				//TODO: appropriate warning message
				delete[] contents;
				return;
			}

			lineHeight = static_cast<real32_t>( face->size->metrics.height ) / 64.0f;

			const size_t numChars = 256u;
			const size_t skip = 0x20u;
			//FIXME: this is just wrong. cell WxH should not be directly related to point size, it does not even imply
			//	an upper bound on the dimensions (also glyph positioning is weird stuff)
			const uint32_t maxCellSize = std::max( pointSize, static_cast<uint16_t>( std::ceil( lineHeight ) ) );
			const size_t atlasSize = numChars * maxCellSize * maxCellSize;
			uint8_t *atlas = new uint8_t[atlasSize];
			std::memset( atlas, 0u, atlasSize );

			console.Print( PrintLevel::Developer, "Generating font atlas for '%s' (%ix%i)\n",
				name.c_str(),
				maxCellSize * charsPerLine,
				maxCellSize * charsPerLine
			);

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
				//TODO: floor/ceil?
				const uint32_t glyphWidth = bitmap.width ? bitmap.width : slot->advance.x / 64;
				const uint32_t glyphHeight = bitmap.rows;

				// atlas will be 16 chars by 16 chars, where chars are WxH but spaced by size*size
				const uint32_t col = c % charsPerLine;
				const uint32_t row = c / charsPerLine;
				const real32_t colPos = static_cast<real32_t>( col ) / static_cast<real32_t>( charsPerLine );
				const real32_t rowPos = static_cast<real32_t>( row ) / static_cast<real32_t>( charsPerLine );
				const size_t rowSize = pointSize * charsPerLine;
				// 2d-space (not pixel) coordinates to the top-left of the desired glyph
				const size_t x = col * maxCellSize; // horizontal position
				const size_t y = row * maxCellSize; // vertical position

				// calculate glyph metrics
				FontData &fd = data[c];
				fd.pixelSize = vector2( glyphWidth, glyphHeight );
				const real32_t stScale = 16.0f * maxCellSize;
				fd.s = vector2( colPos, colPos + (static_cast<real32_t>( glyphWidth ) / stScale) );
				fd.t = vector2( rowPos, rowPos + (static_cast<real32_t>( glyphHeight ) / stScale) );
				fd.advance = static_cast<real32_t>( slot->advance.x ) / 64.0f;
				fd.offset.x = static_cast<real32_t>( slot->metrics.horiBearingX ) / 64.0f;
				fd.offset.y = lineHeight + -(static_cast<real32_t>( slot->metrics.horiBearingY ) / 64.0f);

				if ( !bitmap.buffer ) {
					FT_Done_Glyph( glyph );
					continue;
				}

				// generate an RGBA texture from the 8bpp glyph
				const size_t topLeft = x + (y * rowSize); // row + column
			//	const size_t topRight = y + x + size; // (row + column) + charWidth
			//	const size_t bottomLeft = y + x + (size * rowSize);
			//	const size_t bottomRight = y + x + (size * rowSize) + size;

				for ( size_t y = 0; y < static_cast<size_t>( glyphHeight ); y++ ) {
					for ( size_t x = 0; x < static_cast<size_t>( glyphWidth ); x++ ) {
						atlas[topLeft + (y * rowSize) + x] = bitmap.buffer[y * glyphWidth + x];
					}
				}

				FT_Done_Glyph( glyph );
			}

			FT_Done_Face( face );
			delete[] contents;

			// save out the font atlas
			WritePNG(
				String::Format( "cache/fonts/%s_%i.png", name.c_str(), pointSize ).c_str(),
				atlas,
				maxCellSize * charsPerLine,
				maxCellSize * charsPerLine,
				1
			);

			// and now the glyph metrics
			const File fontdat( String::Format( "cache/fonts/%s_%i.fontdat", name.c_str(), pointSize ).c_str(),
				FileMode::WriteBinary );
			fontdat.Write( &data[skip], sizeof(data) - (sizeof(*data) * skip) );

			texture = new Texture(
				maxCellSize * charsPerLine,
				maxCellSize * charsPerLine,
				InternalFormat::R8,
				atlas
			);
			SDL_assert( texture );

			material = CreateFontMaterial( *texture );
			SDL_assert( material );

			delete[] atlas;
		}

		void Font::Draw( const vector2 &pos, const std::string &text, const vector4 *colour ) {
			if ( text.empty() ) {
				return;
			}

			vector2 currentPos = pos;
			size_t len = text.length();

			for ( size_t i = 0; i < len; i++ ) {
				const char c = text[i];
				const FontData &fd = data[c];

				// check for overflow
				if ( currentPos.x + fd.advance >= state.window.width ) {
					currentPos.x = pos.x;
					currentPos.y += lineHeight;
				}

				// render the glyph
				DrawQuad(
					currentPos.x + fd.offset.x, // x
					currentPos.y + fd.offset.y, // y
					fd.pixelSize.x, // width
					fd.pixelSize.y, // height
					fd.s.x, // s0
					fd.t.x, // t0
					fd.s.y, // s1
					fd.t.y, // t1
					colour,
					material
				);

				// increase by glyph width
				currentPos.x += fd.advance;

				// check for line-feeds
				if ( c == '\n' ) {
					currentPos.x = pos.x;
					currentPos.y += lineHeight;
				}
				//TODO: handle \r? do we just write on top of the previous characters?
				//TODO: handle tabs correctly with tab-stops aligned to 4 spaces - from the window's position, or the
				//	current line's position? use space for alignment? what is the correct behaviour?
			}

			return;
		}

		uint32_t Font::GetTextLineCount( const vector2 &pos, const std::string &text ) {
			uint32_t numLines = 0u;

			if ( text.empty() ) {
				return numLines;
			}

			vector2 currentPos = pos;
			size_t len = text.length();
			const uint32_t screenWidth = vid_width->GetInt();

			for ( size_t i = 0; i < len; i++ ) {
				const char c = text[i];
				const FontData &fd = data[c];

				// check for overflow
				if ( currentPos.x + fd.advance >= screenWidth ) {
					currentPos.x = pos.x;
					currentPos.y += lineHeight;
					numLines++;
				}

				currentPos.x += fd.advance;

				// check for line-feeds
				if ( c == '\n' ) {
					currentPos.x = pos.x;
					currentPos.y += lineHeight;
					numLines++;
				}
			}

			return numLines;
		}

		real32_t Font::GetGlyphWidth( char c ) const {
			const FontData &fd = data[c];
			return fd.advance;
		}

		void Font::Init( void ) {
			if ( FT_Init_FreeType( &freetypeLib ) ) {
				throw( XSError( "Could not initialise freetype library" ) );
			}

			fonts["menu"] = new Font( "menu", 24 );
			fonts["console"] = new Font( "console", 12 );

			static const VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_TexCoord" },
				{ 2, "in_Colour" }
			};

			fontProgram = new ShaderProgram( "text", "text", attributes, ARRAY_LEN( attributes ) );

			for ( const auto &it: fonts ) {
				it.second->RenderGlyphs();
			}
		}

		void Font::Shutdown( void ) {
			for ( const auto &it : fonts ) {
				delete it.second->material;
				delete it.second->texture;
				delete it.second;
			}

			delete fontProgram;
		}

		Font *Font::Register( const char *name, uint16_t pointSize ) {
			Font *font = fonts[name];
			if ( font ) {
				if ( font->pointSize == pointSize ) {
					return font;
				}
				else {
					// size mismatch, we need to regenerate it
					delete font;
				}
			}

			console.Print( PrintLevel::Developer, "Generating new font '%s' at size %i\n", name, pointSize );
			font = fonts[name] = new Font( name, pointSize );
			font->RenderGlyphs();
			return font;
		}

	} // namespace Renderer

} // namespace XS
