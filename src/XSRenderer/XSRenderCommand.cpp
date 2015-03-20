#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSColours.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBuffer.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSMesh.h"
#include "XSRenderer/XSModel.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSVertexAttributes.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSView.h"

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

namespace XS {

	namespace Renderer {

		// render commands are actually backend
		using namespace Backend;

		static Buffer *quadsVertexBuffer;
		static Buffer *quadsIndexBuffer;

		static ShaderProgram *quadProgram = nullptr;
		static Material *quadMaterial = nullptr;
		static Texture *quadTexture = nullptr;

		void RenderCommand::Init( void ) {
			static const uint16_t quadIndices[6] = { 0, 2, 1, 1, 2, 3 };

			// v1(2), v2(2), v3(2), v4(2), st1(2), st2(2), st3(2), st4(2), c1(4) == 20 bytes
			quadsVertexBuffer = new Buffer( BufferType::Vertex, nullptr, 20 * sizeof(real32_t) );
			quadsIndexBuffer = new Buffer( BufferType::Index, quadIndices, sizeof(quadIndices) );

			// create null quad material

			// create texture
			static const size_t numChannels = 4;
			static const size_t textureSize = 8;
			uint8_t textureBuffer[textureSize * textureSize * numChannels] = {};
			std::memset( textureBuffer, 0xFFu, sizeof(textureBuffer) );
			quadTexture = new Texture( textureSize, textureSize, InternalFormat::RGBA8, textureBuffer );

			// create program
			static const VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_TexCoord" },
				{ 2, "in_Colour" }
			};
			quadProgram = new ShaderProgram( "quad", "quad", attributes, ARRAY_LEN( attributes ) );

			// create material
			quadMaterial = new Material();
			Material::SamplerBinding samplerBinding;
			samplerBinding.unit = 0;
			samplerBinding.texture = quadTexture;
			quadMaterial->samplerBindings.push_back( samplerBinding );
			quadMaterial->shaderProgram = quadProgram;
		}

		void RenderCommand::Shutdown( void ) {
			delete quadsVertexBuffer;
			delete quadsIndexBuffer;
			delete quadMaterial;
			delete quadTexture;
			delete quadProgram;
		}

		static void DrawQuad( const DrawQuadCommand &cmd ) {
			if ( cmd.material ) {
				cmd.material->Bind();
			}
			else {
				quadMaterial->Bind();
			}

			vector2 vertices[4];
			vector2 texcoords[4];
			vector4 colour = colourTable[ColourIndex( COLOUR_WHITE )];
			if ( cmd.colour ) {
				colour = *cmd.colour;
			}

			// Top-left
			vertices[0].x	= cmd.x;
			vertices[0].y	= cmd.y;
			texcoords[0].x	= cmd.s1;
			texcoords[0].y	= cmd.t1;

			// Top-right
			vertices[1].x	= cmd.x + cmd.w;
			vertices[1].y	= cmd.y;
			texcoords[1].x	= cmd.s2;
			texcoords[1].y	= cmd.t1;

			// Bottom-left
			vertices[2].x	= cmd.x;
			vertices[2].y	= cmd.y + cmd.h;
			texcoords[2].x	= cmd.s1;
			texcoords[2].y	= cmd.t2;

			// Bottom-right
			vertices[3].x	= cmd.x + cmd.w;
			vertices[3].y	= cmd.y + cmd.h;
			texcoords[3].x	= cmd.s2;
			texcoords[3].y	= cmd.t2;

			real32_t *vertexBuffer = static_cast<real32_t *>( quadsVertexBuffer->Map() );
			for ( size_t i = 0u; i < 4; i++ ) {
				*vertexBuffer++ = vertices[i].x;
				*vertexBuffer++ = vertices[i].y;
				*vertexBuffer++ = texcoords[i].x;
				*vertexBuffer++ = texcoords[i].y;
				*vertexBuffer++ = colour.r;
				*vertexBuffer++ = colour.g;
				*vertexBuffer++ = colour.b;
				*vertexBuffer++ = colour.a;
			}
			quadsVertexBuffer->Unmap();

			quadsIndexBuffer->Bind();

			glEnableVertexAttribArray( 0 );
			glEnableVertexAttribArray( 1 );
			glEnableVertexAttribArray( 2 );
				intptr_t offset = 0;
				const GLsizei stride = sizeof(vector2) + sizeof(vector2) + sizeof(vector4);

				glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, stride, 0 );
				offset += sizeof(vector2);

				glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>( offset ) );
				offset += sizeof(vector2);

				glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>( offset ) );
				offset += sizeof(vector4);

				glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
			glDisableVertexAttribArray( 2 );
			glDisableVertexAttribArray( 1 );
			glDisableVertexAttribArray( 0 );
		}

		static void DrawMesh( const Mesh *mesh ) {
			SDL_assert( mesh->material && "DrawMesh with invalid material" );

			mesh->material->Bind();

			// bind the vertex/normal/uv buffers
			if ( mesh->vertexBuffer ) {
				mesh->vertexBuffer->Bind();
				glEnableVertexAttribArray( 0 );
			//	glEnableVertexAttribArray( 1 );
			//	glEnableVertexAttribArray( 2 );

				size_t offset = 0u;
				const GLsizei stride = sizeof(vector3);

				glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>( offset ) );
				offset += sizeof(vector3);

			//	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>( offset ) );
			//	offset += sizeof(vector3);

			//	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>( offset ) );
			//	offset += sizeof(vector2);
			}

			// issue the draw command
			if ( 0 ) {//mesh->indexBuffer ) {
				mesh->indexBuffer->Bind();
				int size;
				glGetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size );
				glDrawElements( GL_TRIANGLES, size / sizeof(uint16_t), GL_UNSIGNED_SHORT, 0 );
			}
			else {
				glDrawArrays( GL_TRIANGLES, 0, mesh->indices.size() );
			}

			// clean up state
			if ( mesh->vertexBuffer ) {
			//	glDisableVertexAttribArray( 2 );
			//	glDisableVertexAttribArray( 1 );
				glDisableVertexAttribArray( 0 );
			}
		}

		static void DrawModel( const DrawModelCommand &cmd ) {
			for ( const auto &mesh : cmd.model->meshes ) {
				DrawMesh( mesh );
			}
		}

		static void Screenshot( const ScreenshotCommand &cmd ) {
			GLint signalled;

			glGetSynciv( cmd.sync, GL_SYNC_STATUS, 1, NULL, &signalled );
			//TODO: remove this when we wait until next frame
			while ( signalled != GL_SIGNALED ) {
				SDL_Delay( 100 );
				glGetSynciv( cmd.sync, GL_SYNC_STATUS, 1, NULL, &signalled );
			}

			if ( signalled == GL_SIGNALED ) {
				glDeleteSync( cmd.sync );
				glBindBuffer( GL_PIXEL_PACK_BUFFER, cmd.pbo );
				void *data = glMapBuffer( GL_PIXEL_PACK_BUFFER, GL_READ_ONLY );
					console.Print( PrintLevel::Normal,
						"Writing screenshot %s (%ix%i)...\n",
						cmd.name,
						cmd.width,
						cmd.height
					);

					//TODO: strip alpha?
					WritePNG(
						cmd.name,
						reinterpret_cast<uint8_t *>( data ),
						cmd.width,
						cmd.height,
						4
					);
				glUnmapBuffer( GL_PIXEL_PACK_BUFFER );
			}
		}

		void RenderCommand::Execute( void ) const {
			switch( type ) {

			case CommandType::DrawQuad: {
				DrawQuad( drawQuad );
			} break;

			case CommandType::DrawModel: {
				DrawModel( drawModel );
			} break;

			case CommandType::Screenshot: {
				Screenshot( screenshot );
			} break;

			default: {
			} break;

			}
		}

	} // namespace Renderer

} // namespace XS
