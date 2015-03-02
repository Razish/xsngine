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
			size_t size = (sizeof(vector3) * indices.size())/* + (sizeof(vector3) * normals.size())
				+ (sizeof(vector2) * UVs.size())*/;
			vertexBuffer = new Buffer( BufferType::Vertex, nullptr, size );

			real32_t *buffer = static_cast<real32_t *>( vertexBuffer->Map() );
			if ( indices.size() > 0 ) {
				for ( const auto &index : indices ) {
					*buffer++ = vertices[index].x;
					*buffer++ = vertices[index].y;
					*buffer++ = vertices[index].z;
				//	*buffer++ = normals[index].x;
				//	*buffer++ = normals[index].y;
				//	*buffer++ = normals[index].z;
				//	*buffer++ = UVs[index].x;
				//	*buffer++ = UVs[index].y;

					console.Print( PrintLevel::Debug,
						"Pushing %.2f, %.2f, %.2f\n",
						vertices[index].x,
						vertices[index].y,
						vertices[index].z
					);
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
