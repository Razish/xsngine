#include "XSCommon/XSCommon.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"

namespace XS {

	namespace Renderer {

		GLenum GetGLBufferType( BufferType type ) {
			switch ( type ) {
			case BufferType::Vertex:
				return GL_ARRAY_BUFFER;

			case BufferType::Index:
				return GL_ELEMENT_ARRAY_BUFFER;

			case BufferType::Uniform:
				return GL_UNIFORM_BUFFER;

			default:
				return GL_NONE;
			}
		}

		Buffer::Buffer( BufferType type, const void *data, size_t size ) : type(GetGLBufferType( type )), size(size) {
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

		void Material::Bind( void ) const {
			SDL_assert( shaderProgram && "Material::Bind: invalid shader program" );
			shaderProgram->Bind();

			for ( const auto& binding : bufferBindings ) {
				binding.buffer->BindRange( binding.index );
			}

			for ( const auto& binding : samplerBindings ) {
				binding.texture->Bind( binding.unit );
			}
		}

	} // namespace Renderer

} // namespace XS
