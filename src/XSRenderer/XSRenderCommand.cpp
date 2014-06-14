#include "XSSystem/XSInclude.h"

#include <GL/glew.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSVector.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSRenderCommand.h"

namespace XS {

	namespace Renderer {

		static Buffer *quadsVertexBuffer;
		static Buffer *quadsIndexBuffer;

		void RenderCommand::Init() {
			const unsigned short indices[6] = { 0, 2, 1, 1, 2, 3 };

			quadsVertexBuffer = new Buffer( BufferType::Vertex, nullptr, 576 );
			quadsIndexBuffer = new Buffer( BufferType::Index, indices, sizeof(indices) );
		}

		void RenderCommand::Shutdown() {
			delete quadsVertexBuffer;
			delete quadsIndexBuffer;
		}

		static void DrawQuad( const rcDrawQuad_t *quad ) {
			static const vector4 color = vector4( 1.0f, 1.0f, 1.0f, 1.0f );

			quad->material->Bind();

			vector2 vertices[4];
			vector2 texcoords[4];
			vector4 colors[4];

			// Top-left
			vertices[0].x = quad->x;
			vertices[0].y = quad->y;
			texcoords[0].x = quad->s1;
			texcoords[0].y = quad->t1;
			colors[0] = color;

			// Top-right
			vertices[1].x = quad->x + quad->w;
			vertices[1].y = quad->y;
			texcoords[1].x = quad->s2;
			texcoords[1].y = quad->t1;
			colors[1] = color;

			// Bottom-left
			vertices[2].x = quad->x;
			vertices[2].y = quad->y + quad->h;
			texcoords[2].x = quad->s1;
			texcoords[2].y = quad->t2;
			colors[2] = color;

			// Bottom-right
			vertices[3].x = quad->x + quad->w;
			vertices[3].y = quad->y + quad->h;
			texcoords[3].x = quad->s2;
			texcoords[3].y = quad->t2;
			colors[3] = color;

			float *vertexBuffer = static_cast<float *>(quadsVertexBuffer->Map());
			int offset = 0;

			std::memcpy( vertexBuffer, vertices, sizeof(vertices) );
			vertexBuffer += 8;

			std::memcpy( vertexBuffer, texcoords, sizeof(texcoords) );
			vertexBuffer += 8;

			std::memcpy( vertexBuffer, colors, sizeof(colors) );

			quadsVertexBuffer->Unmap();

			quadsIndexBuffer->Bind();

			glEnableVertexAttribArray( 0 );
			glEnableVertexAttribArray( 1 );
			glEnableVertexAttribArray( 2 );

			offset = 0;

			glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
			offset += sizeof(vertices);

			glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid *>(offset) );
			offset += sizeof(texcoords);

			glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<const GLvoid *>(offset) );

			glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
		}

		void RenderCommand::Execute( void ) const {
			switch( type ) {
			case DRAWQUAD:
				DrawQuad( &drawQuad );
				break;
			default:
				break;
			}
		}

	} // namespace Renderer

} // namespace XS
