#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSColours.h"
#include "XSCommon/XSCvar.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBackend.h"
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

		ShaderProgram *quadProgram = nullptr;
		static Material *quadMaterial = nullptr;
		static Texture *quadTexture = nullptr;

		static uint32_t vertexAttribsEnabled = 0;

		enum AttributeIndex {
			VERTEX_ATTRIB_0 = (1 << 0),
			VERTEX_ATTRIB_1 = (1 << 1),
			VERTEX_ATTRIB_2 = (1 << 2),
			VERTEX_ATTRIB_3 = (1 << 3),
			VERTEX_ATTRIB_4 = (1 << 4),
			VERTEX_ATTRIB_5 = (1 << 5),
			VERTEX_ATTRIB_6 = (1 << 6),
			VERTEX_ATTRIB_7 = (1 << 7),
		};

		static void EnableVertexAttribs( uint32_t attribs ) {
			if ( attribs != vertexAttribsEnabled ) {
				uint32_t bit = 1;
				uint32_t bitNumber = 0;

				uint32_t savedAttribs = attribs;
				while ( attribs ) {
					if ( (attribs & 1) && !(vertexAttribsEnabled & bit) ) {
						glEnableVertexAttribArray( bitNumber );
					}
					else if ( !(attribs & 1) && (vertexAttribsEnabled & bit) ) {
						glDisableVertexAttribArray( bitNumber );
					}

					attribs >>= 1;
					bit <<= 1;
					bitNumber++;
				}

				vertexAttribsEnabled = savedAttribs;
			}
		}

		void RenderCommand::Init( void ) {
			static const uint16_t quadIndices[6] = { 0, 2, 1, 1, 2, 3 };

			// 1MB vertex buffer
			// usage strategy is map-discard i.e. keep appending to the buffer until we run out of memory.
			// at this point, orphan the buffer by re-allocating a buffer of the same size and access bits.
			quadsVertexBuffer = new Buffer( BufferType::Vertex, nullptr, 1024 * 1024 );
			quadsIndexBuffer = new Buffer( BufferType::Index, quadIndices, sizeof(quadIndices) );

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

		void DrawQuad( const DrawQuadCommand &cmd ) {
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

			BufferMemory bufferMem = quadsVertexBuffer->MapDiscard( 4 * sizeof( real32_t ) * 8 );
			real32_t *vertexBuffer = static_cast<real32_t *>( bufferMem.devicePtr );
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

			EnableVertexAttribs( VERTEX_ATTRIB_0 | VERTEX_ATTRIB_1 | VERTEX_ATTRIB_2 );
			intptr_t offset = 0;
			const GLsizei stride = sizeof(vector2) + sizeof(vector2) + sizeof(vector4);

			glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, stride,
				reinterpret_cast<const GLvoid *>( bufferMem.offset + offset )
			);
			offset += sizeof(vector2);

			glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, stride,
				reinterpret_cast<const GLvoid *>( bufferMem.offset + offset )
			);
			offset += sizeof(vector2);

			glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, stride,
				reinterpret_cast<const GLvoid *>( bufferMem.offset + offset )
			);
			offset += sizeof(vector4);

			glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
		}

		static void DrawMesh( const Mesh *mesh ) {
			SDL_assert( mesh->material && "DrawMesh with invalid material" );

			mesh->material->Bind();

			bool setWireframe = false;
			bool previousWireframe = false;
			if ( r_wireframe->GetBool() || (mesh->material->flags & MF_WIREFRAME) ) {
				setWireframe = true;
				previousWireframe = GetWireframe();
				ToggleWireframe( true );
			}

			// bind the vertex/normal/uv buffers
			if ( mesh->vertexBuffer ) {
				const bool hasVerts = !mesh->vertices.empty();
				const bool hasNormals = !mesh->normals.empty();
				const bool hasUVs = !mesh->UVs.empty();

				mesh->vertexBuffer->Bind();

				uint32_t vertexAttribs = 0u;
				if ( hasVerts ) {
					vertexAttribs |= VERTEX_ATTRIB_0;
				}
				if ( hasNormals ) {
					vertexAttribs |= VERTEX_ATTRIB_1;
				}
				if ( hasUVs ) {
					vertexAttribs |= VERTEX_ATTRIB_2;
				}

				EnableVertexAttribs( vertexAttribs );

				// calculate stride
				GLsizei stride = 0;
				if ( hasVerts ) {
					stride += sizeof(vector3);
				}
				if ( hasNormals ) {
					stride += sizeof(vector3);
				}
				if ( hasUVs ) {
					stride += sizeof(vector2);
				}

				// set the attribute pointers
				size_t offset = 0u;
				if ( hasVerts ) {
					glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>( offset ) );
					offset += sizeof(vector3);
				}

				if ( hasNormals ) {
					glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>( offset ) );
					offset += sizeof(vector3);
				}

				if ( hasUVs ) {
					glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>( offset ) );
					offset += sizeof(vector2);
				}
			}

			// issue the draw command
			//FIXME: index buffer is all kinds of wrong when using glDrawElements
			if ( 0 ) {//mesh->indexBuffer ) {
				mesh->indexBuffer->Bind();
				GLint size = 0;
				glGetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size );
				glDrawElements( GL_TRIANGLES, size / sizeof(uint32_t), GL_UNSIGNED_INT, 0 );
			}
			else {
				glDrawArrays( GL_TRIANGLES, 0, mesh->indices.size() );
			}

			// clean up state
			if ( setWireframe ) {
				ToggleWireframe( previousWireframe );
			}
		}

		static void DrawModel( const DrawModelCommand &cmd ) {
			for ( const auto &mesh : cmd.model->meshes ) {
				DrawMesh( mesh );
			}
		}

		//TODO: drive this with a generic VBO descriptor to avoid special case drawing like these particles
		static void DrawParticles( const DrawParticlesCommand &cmd ) {
			SDL_assert( cmd.material && "DrawParticles with invalid material" );

			cmd.material->Bind();

			bool setWireframe = false;
			bool previousWireframe = false;
			if ( r_wireframe->GetBool() || (cmd.material->flags & MF_WIREFRAME) ) {
				setWireframe = true;
				previousWireframe = GetWireframe();
				ToggleWireframe( true );
			}

			if ( cmd.vbo ) {
				cmd.vbo->Bind();

				// position4, uv2, colour4
				EnableVertexAttribs( VERTEX_ATTRIB_0 | VERTEX_ATTRIB_1 | VERTEX_ATTRIB_2 );

				// calculate stride
				GLsizei stride = sizeof(vector4) + sizeof(vector2) + sizeof(vector4);

				// set the attribute pointers
				size_t offset = 0u;
				glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>( offset ) );
				offset += sizeof(vector4);

				glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>( offset ) );
				offset += sizeof(vector2);

				glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<const GLvoid *>( offset ) );
				offset += sizeof(vector4);
			}

			if ( 1 ) {
				cmd.ibo->Bind();
				GLint size = 0;
				glGetBufferParameteriv( GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size );
				glDrawElements( GL_TRIANGLES, size / sizeof(uint32_t), GL_UNSIGNED_INT, 0 );
			}
			else {
				glDrawArrays( GL_TRIANGLES, 0, cmd.count );
			}

			// clean up state
			if ( setWireframe ) {
				ToggleWireframe( previousWireframe );
			}
		}

		static void Screenshot( const ScreenshotCommand &cmd ) {
			//NOTE: glGetSynciv never returns GL_SIGNALED on Mac
			// see also: chromium src/ui/gl/gl_fence.cc revision 213908 -> 213907
			//	https://src.chromium.org/viewvc/chrome/trunk/src/ui/gl/gl_fence.cc?r1=213908&r2=213907
			// device details: Yosemite 10.10.3, Intel HD Graphics 5000
#if defined(XS_OS_MAC)
			GLint res = glClientWaitSync( cmd.sync, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED );
			SDL_assert( res != GL_TIMEOUT_EXPIRED );
#else
			GLint signalled = GL_UNSIGNALED;
			do {
				glGetSynciv( cmd.sync, GL_SYNC_STATUS, 1, nullptr, &signalled );
			} while ( signalled != GL_SIGNALED );
#endif

			glDeleteSync( cmd.sync );
			glBindBuffer( GL_PIXEL_PACK_BUFFER, cmd.pbo );
			void *data = glMapBuffer( GL_PIXEL_PACK_BUFFER, GL_READ_ONLY );
				console.Print( PrintLevel::Normal, "Writing screenshot %s (%ix%i)...\n",
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

		void RenderCommand::Execute( void ) const {
			switch ( type ) {

			case CommandType::DrawQuad: {
				DrawQuad( drawQuad );
			} break;

			case CommandType::DrawModel: {
				DrawModel( drawModel );
			} break;

			case CommandType::DrawParticles: {
				DrawParticles( drawParticles );
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
