#include "XSSystem/XSInclude.h"

#include <png.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSRenderer/XSImagePNG.h"

namespace XS {

	namespace Renderer {

	#ifdef _MSC_VER
		#pragma warning( push )
		#pragma warning( disable : 4611 )
	#endif

		void user_read_data( png_structp png_ptr, png_bytep data, png_size_t length );
		void png_print_error( png_structp png_ptr, png_const_charp msg ) {
			Console::Print( "%s\n", msg );
		}

		bool IsPowerOfTwo( int i ) { return (i & (i - 1)) == 0; }

		struct PNGFileReader {
		private:
			byte *buf;
			size_t offset;
			png_structp png_ptr;
			png_infop info_ptr;

		public:
			PNGFileReader( byte *buf ) : buf( buf ), offset( 0 ), png_ptr( NULL ), info_ptr( NULL ){}
			~PNGFileReader() {
				if ( info_ptr ) {
					// destroys both structs
					png_destroy_info_struct( png_ptr, &info_ptr );
				}
				else if ( png_ptr )
					png_destroy_read_struct( &png_ptr, NULL, NULL );
			}

			// returns true on successful read, must delete[] data from caller
			bool Read( byte **data, uint32_t *outWidth, uint32_t *outHeight ) {
				*data = NULL;
				if ( outWidth )
					*outWidth = 0;
				if ( outHeight)
					*outHeight = 0;

				// make sure we're actually reading PNG data.
				const int SIGNATURE_LEN = 8;
				byte ident[SIGNATURE_LEN];
				memcpy( ident, buf, SIGNATURE_LEN );
				if ( !png_check_sig( ident, SIGNATURE_LEN ) ) {
					Console::Print( "PNG signature not found in given image\n" );
					return false;
				}

				png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, png_print_error, png_print_error );
				if ( !png_ptr ) {
					Console::Print( "Could not allocate enough memory to load the image\n" );
					return false;
				}

				info_ptr = png_create_info_struct( png_ptr );
				if ( setjmp( png_jmpbuf( png_ptr ) ) )
					return false;

				// We've read the signature
				offset += SIGNATURE_LEN;

				// Setup reading information, and read header
				png_set_read_fn( png_ptr, (png_voidp)this, &user_read_data );
				png_set_sig_bytes( png_ptr, SIGNATURE_LEN );
				png_read_info( png_ptr, info_ptr );

				png_uint_32 width_;
				png_uint_32 height_;
				int depth;
				int colortype;

				png_get_IHDR( png_ptr, info_ptr, &width_, &height_, &depth, &colortype, NULL, NULL, NULL );

				// While modern OpenGL can handle non-PoT textures, it's faster to handle only PoT
				//	so that the graphics driver doesn't have to fiddle about with the texture when uploading.
				if ( !IsPowerOfTwo( width_ ) || !IsPowerOfTwo( height_ ) ) {
					Console::Print( "Width or height is not a power-of-two.\n" );
					return false;
				}

				// If we need to load a non-RGB(A)8 image, colortype should be PNG_COLOR_TYPE_PALETTE or
				//	PNG_COLOR_TYPE_GRAY.
				if ( colortype != PNG_COLOR_TYPE_RGB && colortype != PNG_COLOR_TYPE_RGBA ) {
					Console::Print( "Image is not 24-bit or 32-bit\n" );
					return false;
				}

				// Read the png data
				// Expand RGB -> RGBA
				if ( colortype == PNG_COLOR_TYPE_RGB )
					png_set_add_alpha( png_ptr, 0xff, PNG_FILLER_AFTER );

				png_read_update_info( png_ptr, info_ptr );

				// We always assume there are 4 channels. RGB channels are expanded to RGBA when read.
				byte *tempData = new byte[width_ * height_ * 4];
				if ( !tempData ) {
					Console::Print( "Could not allocate enough memory to load the image\n" );
					return false;
				}

				// Dynamic array of row pointers, with 'height' elements, initialized to NULL.
				byte **row_pointers = new byte*[sizeof(byte *) * height_];
				if ( !row_pointers ) {
					Console::Print( "Could not allocate enough memory to load the image\n" );
					delete[] tempData;
					return false;
				}

				// Re-set the jmp so that these new memory allocations can be reclaimed
				if ( setjmp( png_jmpbuf( png_ptr ) ) ) {
					delete[] row_pointers;
					delete[] tempData;
					return false;
				}

				for ( unsigned int i=0, j=0; i<height_; i++, j += 4 )
					row_pointers[i] = tempData + j*width_;

				png_read_image( png_ptr, row_pointers );

				// Finish reading
				png_read_end( png_ptr, NULL );
				delete[] row_pointers;

				*data = tempData;
				if ( outWidth )
					*outWidth = width_;
				if ( outHeight )
					*outHeight = height_;

				return true;
			}

			void ReadBytes( void *dest, size_t len ) {
				memcpy( dest, buf + offset, len );
				offset += len;
			}
		};

		void user_read_data( png_structp png_ptr, png_bytep data, png_size_t length ) {
			png_voidp r = png_get_io_ptr( png_ptr );
			PNGFileReader *reader = (PNGFileReader *)r;
			reader->ReadBytes( data, length );
		}

		// Loads a PNG image from file.
		byte *LoadPNG( const char *filename, uint32_t *outWidth, uint32_t *outHeight ) {
			byte *out = NULL;

			const File f = File( filename, FM_READ_BINARY );
			if ( !f.open ) {
				Console::Print( "Could not open PNG file '%s'\n", filename );
				return NULL;
			}

			Console::Print( "Loading '%s'...\n", filename );

			byte *buf = new byte[f.length];
			f.Read( buf );

			PNGFileReader r( buf );
			if ( !r.Read( &out, outWidth, outHeight ) ) {
				delete[] buf;
				return NULL;
			}

			delete[] buf;
			return out;
		}

	#ifdef _MSC_VER
		#pragma warning( pop )
	#endif

	} // namespace Renderer

} // namespace XS
