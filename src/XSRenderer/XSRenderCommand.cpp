#include "XSSystem/XSInclude.h"

#include "GLee/GLee.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSVector.h"
#include "XSRenderer/XSRenderCommand.h"

namespace XS {

	namespace Renderer {

		static void DrawQuad( rcDrawQuad_t *quad ) {
			static vector4 color = vector4( 1.0f, 1.0f, 1.0f, 1.0f );

			glBindTexture( GL_TEXTURE_2D, quad->textureID );

			vector2 vertices[4];
			vector2 texcoords[4];
			vector4 colors[4];
			static const unsigned short indices[6] = { 0, 1, 2, 1, 2, 3 };

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

#if 1
			glEnableClientState( GL_COLOR_ARRAY );
			glVertexPointer( 2, GL_FLOAT, 0, (const GLvoid *)&vertices[0] );
			glTexCoordPointer( 2, GL_FLOAT, 0, (const GLvoid *)&texcoords[0] );
			glColorPointer( 4, GL_FLOAT, 0, (const GLvoid *)&colors[0] );

			glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const GLvoid *)&indices[0] );
			glDisableClientState( GL_COLOR_ARRAY );
#else
			glBegin( GL_TRIANGLES );
				for ( size_t i=0; i<ARRAY_LEN( indices ); i++ ) {
					glTexCoord2fv( &texcoords[indices[i]][0] );
					glColor4fv( &colors[indices[i]][0] );
					glVertex2fv( &vertices[indices[i]][0] );
				}
			glEnd();
#endif

		}

		void RenderCommand::Execute( void ) {
			switch( type ) {
			case RC_DRAWQUAD:
				DrawQuad( &drawQuad );
				break;
			default:
				break;
			}
		}

	} // namespace Renderer

} // namespace XS
