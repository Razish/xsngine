#pragma once

namespace XS {

	namespace Renderer {

		enum class BufferType {
			Vertex,
			Index,
			Uniform
		};

		struct BufferMemory {

			size_t offset;
			size_t size;
			void *devicePtr;

		};

		namespace Backend {

			class Buffer {

			private:
				GLuint	privateID;
				GLenum	type;
				size_t	offset;
				size_t	size;
				size_t	alignment;

			public:
				const GLuint &id;

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

				// ???
				void *Map(
					void
				);

				// ???
				void MapDiscard(
					BufferMemory *buffer,
					size_t size
				);

				// ???
				void Unmap(
					void
				);

				// ???
				void Bind(
					void
				) const;

				// ???
				void BindRange(
					int index,
					size_t offset = 0,
					size_t rangeSize = 0
				) const;

			};

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
