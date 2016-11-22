#include "Common/Common.h"
#include "Common/Cvar.h"
#include "Common/Console.h"
#include "Renderer/Renderer.h"
#include "Renderer/Buffer.h"

namespace Renderer {

	namespace Backend {

		static GLenum GetGLBufferType( BufferType bufferType ) {
			switch ( bufferType ) {
			case BufferType::Vertex: {
				return GL_ARRAY_BUFFER;
			} break;

			case BufferType::Index: {
				return GL_ELEMENT_ARRAY_BUFFER;
			} break;

			case BufferType::Uniform: {
				return GL_UNIFORM_BUFFER;
			} break;

			default: {
				return GL_NONE;
			} break;
			}
		}

		Buffer::Buffer( BufferType bufferType, const void *data, size_t size )
		: type( GetGLBufferType( bufferType ) ), offset( 0 ), size( size ), id( privateID )
		{
			glGenBuffers( 1, &privateID );
			glBindBuffer( type, privateID );
			glBufferData( type, size, data, GL_STREAM_DRAW );

			if ( bufferType == BufferType::Uniform ) {
				glGetIntegerv( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, reinterpret_cast<GLint *>( &alignment ) );
			}
			else {
				alignment = 16;
			}
		}

		Buffer::~Buffer() {
			glDeleteBuffers( 1, &privateID );
		}

		void *Buffer::Map( void ) {
			Bind();
			return glMapBufferRange( type, 0, size, GL_MAP_WRITE_BIT );
		}

		void Buffer::MapDiscard( BufferMemory *result, size_t allocSize ) {
			Bind();

			allocSize = (allocSize + alignment - 1) & ~(alignment - 1);
			if ( (offset + allocSize) > size ) {
				// We've run out of memory. Orphan the buffer and allocate some more memory
				glBufferData( type, size, nullptr, GL_STREAM_DRAW );
				offset = 0;
			}

			result->devicePtr = glMapBufferRange(
				type,
				offset,
				allocSize,
				GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_RANGE_BIT
			);
			result->offset = offset;
			result->size = allocSize;
			offset += allocSize;
		}

		void Buffer::Unmap( void ) {
			glUnmapBuffer( type );
		}

		void Buffer::BindRange( int index, size_t rangeOffset, size_t rangeSize ) const {
			if ( !rangeSize ) {
				rangeSize = size - rangeOffset;
			}

			glBindBufferRange( type, index, privateID, rangeOffset, rangeSize );
		}

		void Buffer::Bind( void ) const {
			glBindBuffer( type, privateID );
		}

	} // namespace Backend

} // namespace Renderer
