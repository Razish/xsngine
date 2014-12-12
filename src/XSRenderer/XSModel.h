#pragma once

#include <vector>
#include <string>

#include "XSRenderer/XSRenderable.h"

namespace XS {

	namespace Renderer {

		class Mesh;

		// shared between all instances of a "model" (i.e. mesh + skin combinations)
		class Model : public Renderable {
		public:
			enum class Type {
				UNKNOWN = 0,
				OBJ,
				NUM_MODEL_TYPES
			};

		private:
			Type type;
			uint32_t refCount;

		public:
			~Model();

			static Model *Register( const char *path );

			virtual bool LoadMeshes( void ) = 0;
			void Draw( void ) const;

			std::string modelPath;
			std::vector<Mesh *> meshes; // will be shared among duplicates to save memory
		};

	} // namespace Renderer

} // namespace XS
