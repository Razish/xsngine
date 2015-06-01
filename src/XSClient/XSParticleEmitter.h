#pragma once

#include <vector>

#include "XSClient/XSEntity.h"
#include "XSRenderer/XSParticle.h"
#include "XSRenderer/XSRenderable.h"

//#define PARTICLE_SORT_ALL

namespace XS {

	namespace Renderer {
		struct Material;
		class ShaderProgram;
		class Texture;

		namespace Backend {
			class Buffer;
		} // namespace Backend
	} // namespace Renderer

	namespace ClientGame {

		class ParticleEmitter : public Entity, public Renderer::Renderable {
		private:
			// only iterate via index (up to numParticles) and treat pool as contiguous memory
			// the list of particles is sorted by alive/dead status to improve performance and avoid the overhead of
			//	object construction/destruction and resizing the pool
			std::vector<Renderer::Particle>	pool;
			uint32_t						numParticles; // used as head
			uint32_t						count;
			uint32_t						life; // in milliseconds
			real64_t						emitTime;
			real64_t						emitRate;

			struct Vertex {
				glm::vec4	position;
				glm::vec2	uv;
				glm::vec4	colour;
			};
			Renderer::Backend::Buffer		*vertexBuffer;
			Renderer::Backend::Buffer		*indexBuffer;

			Renderer::ShaderProgram			*shader;
			Renderer::Texture				*texture;
			Renderer::Material				*material;

#ifdef PARTICLE_SORT_ALL
			// sorts the pool by alive status, then by texture
			void Sort(
				void
			);
#else
			void Swap(
				size_t i
			);
#endif

		public:
			// don't allow default instantiation
			ParticleEmitter() = delete;
			ParticleEmitter( const ParticleEmitter& ) = delete;
			ParticleEmitter& operator=( const ParticleEmitter& ) = delete;

			ParticleEmitter( uint32_t count, uint32_t life, const char *textureName );

			void Update(
				real64_t dt
			);

			void Draw(
				void
			) const;
		};

	} // namespace ClientGame

} // namespace XS
