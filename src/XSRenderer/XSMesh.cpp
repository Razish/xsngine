#include "XSCommon/XSCommon.h"
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
			vertexBuffer = new Buffer( Buffer::Type::VERTEX, nullptr, size );

			real32_t *buffer = static_cast<real32_t *>( vertexBuffer->Map() );
			if ( indices.size() > 0 ) {
				for ( size_t index = 0u; index < indices.size(); index++ ) {
				//for ( auto index : indices ) {
					*buffer++ = vertices[index].x;
					*buffer++ = vertices[index].y;
					*buffer++ = vertices[index].z;
				//	*buffer++ = normals[index].x;
				//	*buffer++ = normals[index].y;
				//	*buffer++ = normals[index].z;
				//	*buffer++ = UVs[index].x;
				//	*buffer++ = UVs[index].y;
				}
			}
			vertexBuffer->Unmap();

			if ( indices.size() > 0 ) {
				indexBuffer = new Buffer( Buffer::Type::INDEX, nullptr, indices.size() * sizeof(uint16_t) );
				uint16_t *buffer = static_cast<uint16_t *>( indexBuffer->Map() );
				for ( auto index : indices ) {
					*buffer++ = index;
				}
				indexBuffer->Unmap();
			}
		}

	} // namespace Renderer

} // namespace XS
