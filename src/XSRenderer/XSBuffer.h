#pragma once

namespace XS {

	namespace Renderer {

		enum class BufferType {
			Vertex,
			Index,
			Uniform
		};

		namespace Backend {

			class Buffer {
			private:
				GLuint	id;
				GLenum	type;
				size_t	size;

			public:
				// don't allow default instantiation
				Buffer() = delete;
				Buffer( const Buffer& ) = delete;
				Buffer& operator=( const Buffer& ) = delete;

				Buffer(
					BufferType bufferType,
					const void *data, // nullptr if you are not yet writing to the buffer
					size_t dataSize // in bytes
				);

				~Buffer();

				void *Map(
					void
				);

				void Unmap(
					void
				);

				GLuint GetID(
					void
				) const;

				void Bind(
					void
				) const;

				void BindRange(
					int index
				) const;

			};

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
