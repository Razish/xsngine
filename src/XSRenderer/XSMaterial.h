#pragma once

#include <GL/glew.h>
#include <vector>

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
		public:
			Buffer () = delete;
			Buffer (const Buffer&) = delete;

			Buffer (BufferType type, const void *data, size_t dataSize);
			~Buffer ();

			void *Map ();
			void Unmap ();

			GLuint GetID () const { return id; }

			void Bind() const;
			void BindRange ( int index ) const;

		private:
			GLuint id;
			GLenum type;
			size_t size;
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

			void Bind () const;
		};

	}

}