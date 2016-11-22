#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSMesh.h"
#include "XSRenderer/XSBuffer.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSVertexAttributes.h"

namespace Renderer {

	using namespace Backend;

	Mesh::~Mesh() {
		delete material;
		delete shader;
		delete texture;
		delete vertexBuffer;
		delete indexBuffer;
	}

	void Mesh::CreateMaterial( void ) {
		if ( materialCreated ) {
			return;
		}

		// create texture
		if ( !texture ) {
			uint8_t *textureBuffer = new uint8_t[128 * 128 * 4];
			std::memset( textureBuffer, 255, sizeof(*textureBuffer) );
			texture = new Texture( 128, 128, InternalFormat::RGBA8, textureBuffer );
			delete[] textureBuffer;
		}

		// create shader program
		static const VertexAttribute attributes[] = {
			{ 0, "in_Position" },
			{ 1, "in_Normal" },
			{ 2, "in_TexCoord" },
		};
		shader = new ShaderProgram( "model", "model", attributes, ARRAY_LEN( attributes ) );

		// create material
		material = new Material();
		Material::SamplerBinding samplerBinding;
		samplerBinding.unit = 0;
		samplerBinding.texture = texture;
		material->samplerBindings.push_back( samplerBinding );
		material->shaderProgram = shader;

		materialCreated = true;
	}

	void Mesh::Upload( void ) {
		if ( uploaded ) {
			return;
		}

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
		if ( indices.size() > 0 ) {
			real32_t *buffer = static_cast<real32_t *>( vertexBuffer->Map() );
			for ( const auto &index : indices ) {
				if ( hasVertices ) {
					*buffer++ = vertices[index][0];
					*buffer++ = vertices[index][1];
					*buffer++ = vertices[index][2];
				}
				if ( hasNormals ) {
					*buffer++ = normals[index][0];
					*buffer++ = normals[index][1];
					*buffer++ = normals[index][2];
				}
				if ( hasUVs ) {
					*buffer++ = UVs[index][0];
					*buffer++ = UVs[index][1];
				}
			}
			vertexBuffer->Unmap();
		}

		if ( indices.size() > 0 ) {
			indexBuffer = new Buffer( BufferType::Index, nullptr, indices.size() * sizeof(uint32_t) );
			uint32_t *buffer = static_cast<uint32_t *>( indexBuffer->Map() );
			for ( const auto &index : indices ) {
				*buffer++ = index;
			}
			indexBuffer->Unmap();
		}

		uploaded = true;
	}

} // namespace Renderer
