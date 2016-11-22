#pragma once

#include <vector>
#include <string>

#include "XSRenderer/XSRenderable.h"

namespace Renderer {

	struct Mesh;

	// shared between all instances of a "model" (i.e. mesh + skin combinations)
	class Model : public Renderable {

	private:
		uint32_t	refCount = 0u; // misnomer, actually counts how many duplicates exist, not how many instances

	public:
		std::string			modelPath;
		std::vector<Mesh *>	meshes; // will be shared among duplicates to save memory

		~Model();

		// register a model
		static Handle Register(
			const char *path
		);

		// issue draw command to renderer
		void Draw(
			const RenderInfo &info
		) const;

		void AddMesh(
			Mesh *mesh
		);

	};

} // namespace Renderer
