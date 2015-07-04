#include <algorithm>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSClient/XSParticleEmitter.h"
#include "XSClient/XSClientGame.h"
#include "XSClient/XSFlyCamera.h"
#include "XSClient/XSClient.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSParticle.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBuffer.h"
#include "XSRenderer/XSVertexAttributes.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSMaterial.h"

namespace XS {

	namespace ClientGame {

		ParticleEmitter::ParticleEmitter( uint32_t count, uint32_t life, const char *textureName )
		: pool( count ), numParticles( 0u ), count( count ), life( life ), emitTime( 0.0 )
		{
			console.Print( PrintLevel::Normal, "%s\n",
				XS_FUNCTION_VERBOSE
			);

			emitRate = static_cast<real32_t>( life ) / static_cast<real32_t>( count );

			// 1MB vertex buffer
			// usage strategy is map-discard i.e. keep appending to the buffer until we run out of memory.
			// at this point, orphan the buffer by re-allocating a buffer of the same size and access bits.
			vertexBuffer = new Renderer::Backend::Buffer(
				Renderer::BufferType::Vertex,
				nullptr,
				count * 4 * sizeof(Vertex)
			);
			indexBuffer = new Renderer::Backend::Buffer(
				Renderer::BufferType::Index,
				nullptr,
				count * 6 * sizeof(uint32_t)
			);

			// material
			material = new Renderer::Material();

			// texture
			uint32_t imgWidth, imgHeight;
			uint8_t *imgData = Renderer::LoadPNG( textureName, &imgWidth, &imgHeight );
			texture = new Renderer::Texture(
				imgWidth, imgHeight,
				Renderer::InternalFormat::RGBA8,
				imgData
			);
			Renderer::Material::SamplerBinding samplerBinding = {};
			samplerBinding.texture = texture;
			material->samplerBindings.push_back( samplerBinding );

			// shader
			static const Renderer::VertexAttribute attributes[] = {
				{ 0, "in_Position" },
				{ 1, "in_TexCoord" },
				{ 2, "in_Colour" }
			};
			shader = new Renderer::ShaderProgram( "particle", "particle", attributes, ARRAY_LEN( attributes ) );
			material->shaderProgram = shader;
		}

		ParticleEmitter::~ParticleEmitter() {
			console.Print( PrintLevel::Normal, "%s\n",
				XS_FUNCTION_VERBOSE
			);
		}

#ifdef PARTICLE_SORT_ALL
		int particlecmp_alive( const Renderer::Particle &p1, const Renderer::Particle &p2 ) {
			if ( p1.life >= 0 && p2.life >= 0 ) {
				return 0;
			}
			return p1.life < p2.life ? -1 : 1;
		}

		int particlecmp_life( const Renderer::Particle &p1, const Renderer::Particle &p2 ) {
			if ( p1.life == p2.life ) {
				return 0;
			}
			return p1.life < p2.life ? -1 : 1;
		}

		int particlecmp_texture( const Renderer::Particle &p1, const Renderer::Particle &p2 ) {
			if ( !p1.texture ) {
				if ( !p2.texture ) {
					return 0;
				}
				return -1;
			}
			else if ( !p2.texture ) {
				return 1;
			}

			if ( p1.texture->id == p2.texture->id ) {
				return 0;
			}
			return p1.texture->id < p2.texture->id ? -1 : 1;
		}

		void ParticleEmitter::Sort( void ) {
			// sort by alive status
			std::sort( pool.begin(), pool.end(), particlecmp_alive );

			// sort by amount of life left
		//	std::sort( pool.begin(), pool.end(), particlecmp_life );

			// sort by texture
			std::sort( pool.begin(), pool.end(), particlecmp_texture );
		}
#else
		void ParticleEmitter::Swap( size_t i ) {
			// temp swap
			Renderer::Particle tmp = pool[i];
			pool[i] = pool[numParticles - 1];
			pool[numParticles-1] = tmp;
		}
#endif

		void ParticleEmitter::Update( real64_t dt ) {
			//FIXME: optimise with better container and investigate swap vs resize performance

			// simulate all particles
			for ( size_t i = 0u; i < numParticles; i++ ) {
				auto &p = pool[i];
				if ( !p.Update( dt ) ) {
					// instead of removing this particle, swap it with the last active particle
					// to keep the dead particles at the end, in contiguous memory
					Swap( i );
					i--;
					numParticles--;
					continue;
				}
			}

#ifdef PARTICLE_SORT_ALL
			Sort();
#endif

			// see if we need to spawn more
			if ( emitTime >= emitRate && numParticles < count ) {
				emitTime -= emitRate;
			//	SDL_assert( numParticles < count );
				pool[numParticles] = Renderer::Particle();
				pool[numParticles].life = life;
				numParticles++;
			}
			emitTime += dt;
		}

		void ParticleEmitter::Draw( const Renderer::RenderInfo &info ) const {
			uint32_t *indexMem = new uint32_t[count * 6]();
			Vertex *vertexMem = new Vertex[count * 4]();
			for ( size_t i = 0u; i < numParticles; i++ ) {
				auto &p = pool[i];

				indexMem[i * 6 + 0] = i * 4 + 0;
				indexMem[i * 6 + 1] = i * 4 + 1;
				indexMem[i * 6 + 2] = i * 4 + 2;

				indexMem[i * 6 + 3] = i * 4 + 0;
				indexMem[i * 6 + 4] = i * 4 + 2;
				indexMem[i * 6 + 5] = i * 4 + 3;

				// update the vertex buffer
				const real32_t halfSize = p.size / 2.0f;
				const glm::vec4 particleColour( p.colour.x, p.colour.y, p.colour.z, p.colour.w );
				vertexMem[i * 4 + 0].position = glm::vec4(  halfSize,  halfSize, 0.0f, 1.0f );
				vertexMem[i * 4 + 0].uv = glm::vec2( 1.0, 1.0f );
				vertexMem[i * 4 + 0].colour = particleColour;

				vertexMem[i * 4 + 1].position = glm::vec4( -halfSize,  halfSize, 0.0f, 1.0f );
				vertexMem[i * 4 + 1].uv = glm::vec2( 0.0, 1.0f );
				vertexMem[i * 4 + 1].colour = particleColour;

				vertexMem[i * 4 + 2].position = glm::vec4( -halfSize, -halfSize, 0.0f, 1.0f );
				vertexMem[i * 4 + 2].uv = glm::vec2( 0.0, 0.0f );
				vertexMem[i * 4 + 2].colour = particleColour;

				vertexMem[i * 4 + 3].position = glm::vec4(  halfSize, -halfSize, 0.0f, 1.0f );
				vertexMem[i * 4 + 3].uv = glm::vec2( 1.0, 0.0f );
				vertexMem[i * 4 + 3].colour = particleColour;


				const glm::mat4 &cameraTransform = camera->GetTransform();
				const glm::vec3 particlePosition( p.position.x, p.position.y, p.position.z );
				glm::vec3 zAxis = glm::normalize( glm::vec3( cameraTransform[3] ) - particlePosition );
				glm::vec3 xAxis = glm::cross( glm::vec3( cameraTransform[1] ), zAxis );
				glm::vec3 yAxis = glm::cross( zAxis, xAxis );
				glm::mat4 billboard(
					glm::vec4( xAxis, 0 ),
					glm::vec4( yAxis, 0 ),
					glm::vec4( zAxis, 0 ),
					glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f )
				);

				vertexMem[i * 4 + 0].position = billboard
					* vertexMem[i * 4 + 0].position + glm::vec4( p.position, 0.0f );
				vertexMem[i * 4 + 1].position = billboard
					* vertexMem[i * 4 + 1].position + glm::vec4( p.position, 0.0f );
				vertexMem[i * 4 + 2].position = billboard
					* vertexMem[i * 4 + 2].position + glm::vec4( p.position, 0.0f );
				vertexMem[i * 4 + 3].position = billboard
					* vertexMem[i * 4 + 3].position + glm::vec4( p.position, 0.0f );
			}

			Vertex *vertexData = reinterpret_cast<Vertex *>( vertexBuffer->Map() );
			std::memcpy( vertexData, vertexMem, count * 4 * sizeof(Vertex) );
			vertexBuffer->Unmap();
			delete[] vertexMem;

			uint32_t *indexData = reinterpret_cast<uint32_t *>( indexBuffer->Map() );
			std::memcpy( indexData, indexMem, count * 6 * sizeof(uint32_t) );
			indexBuffer->Unmap();
			delete[] indexMem;

			Renderer::DrawParticles( vertexBuffer, indexBuffer, material, numParticles * 6 );
		}

	} // namespace Renderer

} // namespace XS
