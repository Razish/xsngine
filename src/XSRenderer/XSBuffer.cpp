#include "XSCommon/XSCommon.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBuffer.h"

namespace XS {

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

			GLuint Buffer::GetID( void ) const {
				return id;
			}

			Buffer::Buffer( BufferType bufferType, const void *data, size_t size )
			: type( GetGLBufferType( bufferType ) ), size( size )
			{
				glGenBuffers( 1, &id );
				glBindBuffer( type, id );
				glBufferData( type, size, data, GL_STATIC_DRAW );
			}

			Buffer::~Buffer() {
				glDeleteBuffers( 1, &id );
			}

			void *Buffer::Map( void ) {
				Bind();
				return glMapBufferRange( type, 0, size, GL_MAP_WRITE_BIT );
			}

			void Buffer::Unmap( void ) {
				glUnmapBuffer( type );
			}

			void Buffer::BindRange( int index ) const {
				glBindBufferRange( type, index, id, 0, size );
			}

			void Buffer::Bind( void ) const {
				glBindBuffer( type, id );
			}

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
