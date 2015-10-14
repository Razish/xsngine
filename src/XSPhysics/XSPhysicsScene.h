#pragma once

#include <ode/ode.h>
#include <vector>

namespace XS {

	namespace Physics {

		// uses the same coordinate system as the renderer (i.e. +Y is up)

		struct Object {
			dBodyID		body;
			dGeomID		geom;

			Object();
		};

		extern class Scene {
		private:
			dWorldID			world;
			dSpaceID			space;
			dJointGroupID		contactGroup;
			std::vector<Object>	objects;

		public:
			Scene();
			~Scene();

			dWorldID GetWorld(
				void
			) const;

			dSpaceID GetSpace(
				void
			) const;
		} scene;

	} // namespace Physics

} // namespace XS
