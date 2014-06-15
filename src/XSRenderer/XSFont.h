#pragma once

#include "XSCommon/XSVector.h"

namespace XS {

	namespace Renderer {

		struct font_t {
			font_t() : file( "" ), size( 24 ) {}
			font_t( const char *name, uint16_t size );
			std::string		file;
			uint16_t		size;
			struct {
				//FIXME: XSTexture
				uint32_t		texture;
				vector4			dimensions;
			} glyph[0xff];
		};

		// should not be instantiated
		class Font {
		private:
			Font();

		public:
			static void Init( void );
			static void Shutdown( void );
			static font_t *Register( const std::string &name, uint16_t size );
			static void Draw( const vector2 pos, const std::string &text, const font_t *font );
		};

	} // namespace Renderer

} // namespace XS
