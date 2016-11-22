#pragma once

#include <vector>

#include "Client/Entity.h"
#include "Renderer/Particle.h"
#include "Renderer/Renderable.h"

//#define PARTICLE_SORT_ALL

namespace Renderer {
	struct Material;
	class ShaderProgram;
	class Texture;

	namespace Backend {
		class Buffer;
	} // namespace Backend
} // namespace Renderer

namespace ClientGame {

	class ParticleEmitter : public Renderer::Renderable {
	private:
		// only iterate via index (up to numParticles) and treat pool as contiguous memory
		// the list of particles is sorted by alive/dead status to improve performance and avoid the overhead of
		//	object construction/destruction and resizing the pool
		std::vector<Renderer::Particle>	pool;
		uint32_t						numParticles = 0u; // used as head
		uint32_t						count = 10u;
		uint32_t						life = 1000u; // in milliseconds
		real64_t						emitTime = 1.0;
		real64_t						emitRate = 1.0;

		struct Vertex {
			glm::vec4	position;
			glm::vec2	uv;
			glm::vec4	colour;
		};
		Renderer::Backend::Buffer		*vertexBuffer = nullptr;
		Renderer::Backend::Buffer		*indexBuffer = nullptr;

		Renderer::ShaderProgram			*shader = nullptr;
		Renderer::Texture				*texture = nullptr;
		Renderer::Material				*material = nullptr;

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
		~ParticleEmitter();

		void Update(
			real64_t dt
		);

		void Draw(
			const Renderer::RenderInfo &info
		) const;
	};

} // namespace ClientGame
