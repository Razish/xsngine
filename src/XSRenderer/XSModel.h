#pragma once

#include <vector>
#include <string>

#include "XSRenderer/XSRenderable.h"

namespace XS {

	namespace Renderer {

		struct Mesh;

		// shared between all instances of a "model" (i.e. mesh + skin combinations)
		class Model : public Renderable {
		public:
			enum class Type {
				UNKNOWN = 0,
				OBJ,
				XMF,
				NUM_MODEL_TYPES
			};

		private:
			Type		type;
			uint32_t	refCount;

		public:
			std::string			modelPath;
			std::vector<Mesh *>	meshes; // will be shared among duplicates to save memory

			~Model();

			// register a model
			// type is inferred by the file extension
			static Model *Register(
				const char *path
			);

			// load the meshes associated with the model and upload to GPU
			virtual bool LoadMeshes(
				void
			) = 0;

			// issue draw command to renderer
			void Draw(
				void
			) const;
		};

	} // namespace Renderer

} // namespace XS
