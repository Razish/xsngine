#pragma once

#include <GL/glew.h>

namespace XS {

	namespace Renderer {

		class ShaderProgram;
		class Texture;

		enum class BufferType {
			Vertex,
			Index,
			Uniform
		};

		class Buffer {
		private:
			GLuint id;
			GLenum type;
			size_t size;

		public:
			// don't allow default instantiation
			Buffer() = delete;
			Buffer( const Buffer& ) = delete;
			Buffer& operator=( const Buffer& ) = delete;

			Buffer( BufferType type, const void *data, size_t dataSize );
			~Buffer();

			void *Map( void );
			void Unmap( void );

			GLuint GetID() const { return id; }

			void Bind( void ) const;
			void BindRange( int index ) const;

		};

		struct Material {
			struct SamplerBinding {
				int unit;
				Texture *texture;
			};

			struct BufferBinding {
				int index;
				Buffer *buffer;
			};

			ShaderProgram *shaderProgram;
			std::vector<SamplerBinding> samplerBindings;
			std::vector<BufferBinding> bufferBindings;

			void Bind( void ) const;
		};

	} // namespace Renderer

} // namespace XS
