#pragma once

#include <vector>
#include <string>

#include "XSRenderer/XSRenderable.h"

namespace XS {

	namespace Renderer {

		struct Mesh;

		enum class ModelType {
			Invalid,
			OBJ,
			XMF,
			RAW,
			NUM_MODEL_TYPES
		};

		// shared between all instances of a "model" (i.e. mesh + skin combinations)
		class Model : public Renderable {
		private:
			ModelType	type;
			uint32_t	refCount = 0u; // misnomer, actually counts how many duplicates exist, not how many instances

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
				const RenderInfo &info
			) const;
		};

		class Raw : public Model {

			// ???
			inline bool LoadMeshes( void ) {
				return true;
			}

		};

	} // namespace Renderer

} // namespace XS
