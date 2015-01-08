#include "XSCommon/XSCommon.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSMesh.h"
#include "XSRenderer/XSBuffer.h"

namespace XS {

	namespace Renderer {

		using namespace Backend;

		void Mesh::Upload( void ) {
			size_t size = (sizeof(vector3) * vertices.size()) + (sizeof(vector3) * normals.size())
				+ (sizeof(vector2) * UVs.size());
			vertexBuffer = new Buffer( Buffer::Type::VERTEX, nullptr, size );

			float *buffer = static_cast<float *>( vertexBuffer->Map() );
			if ( vertices.size() > 0 ) {
				for ( size_t i = 0; i < vertices.size(); i++ ) {
					*buffer++ = vertices[i].x;
					*buffer++ = vertices[i].y;
					*buffer++ = vertices[i].z;
				}
			}

			if ( normals.size() > 0 ) {
				for ( size_t i = 0; i < normals.size(); i++ ) {
					*buffer++ = normals[i].x;
					*buffer++ = normals[i].y;
					*buffer++ = normals[i].z;
				}
			}

			if ( UVs.size() > 0 ) {
				for ( size_t i = 0; i < UVs.size(); i++ ) {
					*buffer++ = UVs[i].x;
					*buffer++ = UVs[i].y;
				}
			}
			vertexBuffer->Unmap();

			if ( indices.size() > 0 ) {
				indexBuffer = new Buffer( Buffer::Type::INDEX, nullptr, indices.size() * sizeof(vector2) );
				float *buffer = static_cast<float *>( indexBuffer->Map() );
				for ( size_t i = 0; i < indices.size(); i++ ) {
					*buffer = indices[i];
					buffer++;
				}
				indexBuffer->Unmap();
			}
		}

	} // namespace Renderer

} // namespace XS
