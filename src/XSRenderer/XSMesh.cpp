#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSMesh.h"
#include "XSRenderer/XSBuffer.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"

namespace XS {

	namespace Renderer {

		using namespace Backend;

		Mesh::~Mesh() {
			delete material;
			delete shader;
			delete texture;
			delete vertexBuffer;
			delete indexBuffer;
		}

		void Mesh::Upload( void ) {
			size_t size = 0u;
			if ( indices.size() ) {
				size += sizeof(vector3) * indices.size();
			}
			if ( normals.size() ) {
				size += sizeof(vector3) * indices.size();
			}
			if ( UVs.size() ) {
				size += sizeof(vector2) * indices.size();
			}
			vertexBuffer = new Buffer( BufferType::Vertex, nullptr, size );

			bool hasVertices = indices.size() > 0u;
			bool hasNormals = normals.size() > 0u;
			bool hasUVs = UVs.size() > 0u;
			real32_t *buffer = static_cast<real32_t *>( vertexBuffer->Map() );
			if ( indices.size() > 0 ) {
				for ( const auto &index : indices ) {
					if ( hasVertices ) {
						*buffer++ = vertices[index].x;
						*buffer++ = vertices[index].y;
						*buffer++ = vertices[index].z;
					}
					if ( hasNormals ) {
						*buffer++ = normals[index].x;
						*buffer++ = normals[index].y;
						*buffer++ = normals[index].z;
					}
					if ( hasUVs ) {
						*buffer++ = UVs[index].x;
						*buffer++ = UVs[index].y;
					}
				}
			}
			vertexBuffer->Unmap();

			if ( indices.size() > 0 ) {
				indexBuffer = new Buffer( BufferType::Index, nullptr, indices.size() * sizeof(uint16_t) );
				uint16_t *buffer = static_cast<uint16_t *>( indexBuffer->Map() );
				for ( auto index : indices ) {
					*buffer++ = index;
				}
				indexBuffer->Unmap();
			}
		}

	} // namespace Renderer

} // namespace XS
