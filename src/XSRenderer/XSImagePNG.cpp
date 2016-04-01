#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <png.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSImagePNG.h"

namespace XS {

	namespace Renderer {

	#ifdef _MSC_VER
		#pragma warning( push )
		#pragma warning( disable : 4611 )
	#endif

		void user_read_data( png_structp png_ptr, png_bytep data, png_size_t length );
		void user_write_data( png_structp png_ptr, png_bytep data, png_size_t length );
		void user_flush_data( png_structp png_ptr );

		void png_print_error( png_structp png_ptr, png_const_charp msg ) {
			console.Print( PrintLevel::Developer, "%s\n",
				msg
			);
		}

		bool IsPowerOfTwo( int i ) {
			return (i & (i - 1)) == 0;
		}

		struct PNGFileReader {
		private:
			uint8_t *buf;
			size_t offset;
			png_structp png_ptr;
			png_infop info_ptr;

		public:
			PNGFileReader( uint8_t *buf )
			: buf( buf ), offset( 0 ), png_ptr( nullptr ), info_ptr( nullptr )
			{
			}

			~PNGFileReader() {
				if ( info_ptr ) {
					// destroys both structs
					png_destroy_info_struct( png_ptr, &info_ptr );
				}
				else if ( png_ptr ) {
					png_destroy_read_struct( &png_ptr, nullptr, nullptr );
				}
			}

			// returns true on successful read, must delete[] data from caller
			//	outWidth and outHeight will be filled in with the image's width/height. they are not read or used for
			//	scaling
			bool Read( uint8_t **data, uint32_t *outWidth, uint32_t *outHeight ) {
				*data = nullptr;

				// initialise the width/height to 0 so we know if reading succeeded
				if ( outWidth ) {
					*outWidth = 0u;
				}
				if ( outHeight ) {
					*outHeight = 0u;
				}

				// make sure we're actually reading PNG data.
				const size_t signatureLen = 8;
				uint8_t ident[signatureLen] = {};
				std::memcpy( ident, buf, signatureLen );
				if ( !png_check_sig( ident, signatureLen ) ) {
					console.Print( PrintLevel::Developer, "PNG signature not found in given image\n" );
					return false;
				}

				png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, nullptr, png_print_error, png_print_error );
				if ( !png_ptr ) {
					console.Print( PrintLevel::Developer, "Could not allocate enough memory to load the image\n" );
					return false;
				}

				info_ptr = png_create_info_struct( png_ptr );
				if ( setjmp( png_jmpbuf( png_ptr ) ) ) {
					return false;
				}

				// we've read the signature
				offset += signatureLen;

				// setup reading information, and read header
				// the io_ptr simply points to this class instance, and our user_read_data function will cast it back
				png_set_read_fn( png_ptr, (png_voidp)this, &user_read_data );
				png_set_sig_bytes( png_ptr, signatureLen );
				png_read_info( png_ptr, info_ptr );

				png_uint_32 width_, height_;
				int depth, colortype;
				png_get_IHDR( png_ptr, info_ptr, &width_, &height_, &depth, &colortype, nullptr, nullptr, nullptr );

				// while modern OpenGL can handle non-PoT textures, it's faster to handle only PoT so that the graphics
				//	driver doesn't have to fiddle about with the texture when uploading.
				if ( !IsPowerOfTwo( width_ ) || !IsPowerOfTwo( height_ ) ) {
					console.Print( PrintLevel::Normal, "Width or height is not a power-of-two.\n" );
					return false;
				}

				// if we need to load a non-RGB(A)8 image, colortype should be PNG_COLOR_TYPE_PALETTE or
				//	PNG_COLOR_TYPE_GRAY.
				if ( colortype != PNG_COLOR_TYPE_RGB && colortype != PNG_COLOR_TYPE_RGBA ) {
					console.Print( PrintLevel::Normal, "Image is not 24-bit or 32-bit\n" );
					return false;
				}

				// read the png data
				// expand from RGB -> RGBA if necessary, filling alpha channel with 255 for full opacity
				if ( colortype == PNG_COLOR_TYPE_RGB ) {
					png_set_add_alpha( png_ptr, 0xff, PNG_FILLER_AFTER );
				}

				png_read_update_info( png_ptr, info_ptr );

				// we always assume there are 4 channels. RGB channels are expanded to RGBA when read.
				uint8_t *tempData = new uint8_t[width_ * height_ * 4];
				if ( !tempData ) {
					console.Print( PrintLevel::Normal, "Could not allocate enough memory to load the image\n" );
					return false;
				}

				// dynamic array of row pointers, with 'height' elements, initialized to NULL.
				uint8_t **row_pointers = new uint8_t*[sizeof(uint8_t *) * height_];
				if ( !row_pointers ) {
					console.Print( PrintLevel::Normal, "Could not allocate enough memory to load the image\n" );
					delete[] tempData;
					return false;
				}

				// re-set the jmp so that these new memory allocations can be reclaimed
				if ( setjmp( png_jmpbuf( png_ptr ) ) ) {
					delete[] row_pointers;
					delete[] tempData;
					return false;
				}

				for ( size_t i = 0u, j = 0u; i < height_; i++, j += 4 ) {
					row_pointers[i] = tempData + j*width_;
				}

				png_read_image( png_ptr, row_pointers );

				// finish reading
				png_read_end( png_ptr, nullptr );
				delete[] row_pointers;

				*data = tempData;
				if ( outWidth ) {
					*outWidth = width_;
				}
				if ( outHeight ) {
					*outHeight = height_;
				}

				return true;
			}

			void ReadBytes( void *dest, size_t len ) {
				memcpy( dest, buf + offset, len );
				offset += len;
			}

		};

		struct PNGFileWriter {
		private:
			size_t offset;
			png_structp png_ptr;
			png_infop info_ptr;

		public:
			File &f;
			ByteBuffer bb;

			PNGFileWriter( File &f )
			: offset( 0 ), png_ptr( nullptr ), info_ptr( nullptr ), f( f )
			{
			}

			~PNGFileWriter() {
				// because libpng is teh sux
				size_t outLen = 0u;
				const void *mem = bb.GetMemory( &outLen );
				f.Write( mem, outLen );

				if ( info_ptr ) {
					// destroys both structs
					png_destroy_info_struct( png_ptr, &info_ptr );
				}
				else if ( png_ptr ) {
					png_destroy_write_struct( &png_ptr, nullptr );
				}
			}

			// returns true on successful write
			bool Write( uint8_t *data, uint32_t w, uint32_t h, int numChannels ) {
				png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, nullptr, png_print_error,
					png_print_error
				);
				if ( !png_ptr ) {
					return false;
				}

				info_ptr = png_create_info_struct( png_ptr );
				if ( !info_ptr ) {
					png_destroy_write_struct( &png_ptr, &info_ptr );
					return false;
				}

				if ( setjmp( png_jmpbuf( png_ptr ) ) ) {
					return false;
				}

				// the io_ptr simply points to this class instance, and our user_write_data function will cast it back
				png_set_write_fn( png_ptr, (png_voidp)this, &user_write_data, &user_flush_data );

				int colourType = PNG_COLOR_TYPE_RGB;
				if ( numChannels == 4 ) {
					colourType = PNG_COLOR_TYPE_RGBA;
				}
				else if ( numChannels == 1 ) {
					colourType = PNG_COLOR_TYPE_GRAY;
				}
				else if ( numChannels != 3 ) {
					SDL_assert( !"WritePNG: invalid numChannels, expected 1, 3 or 4" );
				}

				png_set_IHDR( png_ptr, info_ptr, w, h, 8/*depth*/, colourType, PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_BASE,
					PNG_FILTER_TYPE_BASE
				);
				png_colorp palette = NULL;
				if ( numChannels != 1 ) {
					palette = static_cast<png_colorp>(
						png_malloc( png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof(png_color) )
					);
					if ( !palette ) {
						png_destroy_write_struct( &png_ptr, &info_ptr );
						return false;
					}
					png_set_PLTE( png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH );
				}
				png_write_info( png_ptr, info_ptr );
				png_set_packing( png_ptr );

				png_bytepp rows = static_cast<png_bytepp>( png_malloc( png_ptr, h * sizeof(png_bytep) ) );
				std::memset( rows, 0, h * sizeof(png_bytep) );
				for ( int i = 0; i < h; i++ ) {
					rows[i] = (png_bytep)(data + (h - i - 1) * w * numChannels);
				}

				// re-set the jmp so that these new memory allocations can be reclaimed
				if ( setjmp( png_jmpbuf( png_ptr ) ) ) {
					png_free( png_ptr, rows );
					png_free( png_ptr, palette );
					return false;
				}

				png_write_image( png_ptr, rows );
				png_write_end( png_ptr, info_ptr );
				//png_write_flush( png_ptr );
				png_free( png_ptr, palette );
				png_destroy_write_struct( &png_ptr, &info_ptr );

				png_free( png_ptr, rows );
				return true;
			}

			void WriteBytes( void *data, size_t len ) {
				//f.Write( static_cast<uint8_t *>( data ) + offset, len, true );
				ByteBuffer::Error status = bb.WriteRaw( data, len );
				if ( status == ByteBuffer::Error::Success ) {
					offset += len;
				}
				else {
					console.Print( PrintLevel::Developer, "%s(%s): Failed to write %" PRIuMAX " bytes\n",
						XS_FUNCTION, f.path, len
					);
				}
			}

		};

		void user_read_data( png_structp png_ptr, png_bytep data, png_size_t length ) {
			png_voidp r = png_get_io_ptr( png_ptr );
			PNGFileReader *reader = static_cast<PNGFileReader *>( r );
			reader->ReadBytes( data, length );
		}

		void user_write_data( png_structp png_ptr, png_bytep data, png_size_t length ) {
			png_voidp w = png_get_io_ptr( png_ptr );
			PNGFileWriter *writer = static_cast<PNGFileWriter *>( w );
			writer->WriteBytes( data, length );
		}

		void user_flush_data( png_structp png_ptr ) {
			png_voidp w = png_get_io_ptr( png_ptr );
			PNGFileWriter *writer = static_cast<PNGFileWriter *>( w );
			size_t outLen = 0u;
			const void *mem = writer->bb.GetMemory( &outLen );
			writer->f.Write( mem, outLen );
			console.Print( PrintLevel::Developer, "%s(%s): writing %" PRIuMAX " bytes from 0x%" PRIXPTR "\n",
				XS_FUNCTION, writer->f.path, outLen, mem
			);
		}

		// Loads a PNG image from file.
		uint8_t *LoadPNG( const char *filename, uint32_t *outWidth, uint32_t *outHeight ) {
			uint8_t *out = nullptr;

			const File f( filename, FileMode::ReadBinary );
			if ( !f.open ) {
				console.Print( PrintLevel::Normal, "Could not open PNG file \"%s\" for reading\n", filename );
				return nullptr;
			}

			console.Print( PrintLevel::Developer, "%s( %s )\n",
				XS_FUNCTION,
				filename
			);

			uint8_t *buf = new uint8_t[f.length];
			std::memset( buf, 0, f.length );
			f.Read( buf );

			PNGFileReader reader( buf );
			if ( !reader.Read( &out, outWidth, outHeight ) ) {
				delete[] buf;
				return nullptr;
			}

			delete[] buf;
			return out;
		}

		bool WritePNG( const char *filename, uint8_t *pixels, int w, int h, int numChannels ) {
			File f( filename, FileMode::WriteBinary );
			if ( !f.open ) {
				console.Print( PrintLevel::Normal, "Could not open PNG file \"%s\" for writing\n", filename );
				return false;
			}

			PNGFileWriter writer( f );
			if ( !writer.Write( pixels, w, h, numChannels ) ) {
				return false;
			}

			return true;
		}

	#ifdef _MSC_VER
		#pragma warning( pop )
	#endif

	} // namespace Renderer

} // namespace XS
