#pragma once

#include <GL/glew.h>

namespace XS {

	namespace Renderer {

		namespace Backend {

			class Buffer {
			public:
				enum class Type {
					VERTEX,
					INDEX,
					UNIFORM
				};

			private:
				GLuint	id;
				GLenum	type;
				size_t	size;

			public:
				Buffer( Type type, const void *data, size_t dataSize );

				// don't allow default instantiation
				Buffer() = delete;
				Buffer( const Buffer& ) = delete;
				Buffer& operator=( const Buffer& ) = delete;

				~Buffer();

				void	*Map		( void );
				void	 Unmap		( void );

				GLuint	 GetID		( void ) const;

				void	 Bind		( void ) const;
				void	 BindRange	( int index ) const;

			};

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
