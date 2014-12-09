#include "XSCommon/XSCommon.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBuffer.h"

namespace XS {

	namespace Renderer {

		namespace Backend {

			static GLenum GetGLBufferType( Buffer::Type type ) {
				switch ( type ) {
				case Buffer::Type::VERTEX:
					return GL_ARRAY_BUFFER;

				case Buffer::Type::INDEX:
					return GL_ELEMENT_ARRAY_BUFFER;

				case Buffer::Type::UNIFORM:
					return GL_UNIFORM_BUFFER;

				default:
					return GL_NONE;
				}
			}

			GLuint Buffer::GetID( void ) const {
				return id;
			}

			Buffer::Buffer( Type type, const void *data, size_t size )
			: type( GetGLBufferType( type ) ), size( size )
			{
				glGenBuffers( 1, &id );
				glBindBuffer( this->type, id );
				glBufferData( this->type, size, data, GL_STREAM_DRAW );
			}

			Buffer::~Buffer() {
				glDeleteBuffers( 1, &id );
			}

			void *Buffer::Map( void ) {
				glBindBuffer( type, id );
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
