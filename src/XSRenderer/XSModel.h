#pragma once

#include "XSRenderer/XSRenderable.h"

namespace XS {

	namespace Renderer {

		// shared between all instances of a "model" (i.e. mesh + skin combinations)
		class Model : public Renderable {
		private:
			uint32_t id;
			std::vector<Mesh *> meshes;

		public:
			enum class Type {
				UNKNOWN = 0,
				OBJ,
				NUM_MODEL_TYPES
			};

			~Model();

			// will add associated meshes
			static Model *Register( const char *path );

			Type type;
		};

	} // namespace Renderer

} // namespace XS
