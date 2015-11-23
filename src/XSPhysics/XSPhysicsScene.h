#pragma once

#include <ode/ode.h>
#include <vector>

namespace XS {

	namespace Physics {

		// uses the same coordinate system as the renderer (i.e. +Y is up)

		struct Object {
			dBodyID		body;
			dGeomID		geom;
			dMass		mass;

			char		*geomData = nullptr;

			uint32_t	id;
			static uint32_t numObjects;

			Object();
			~Object();
		};

		extern class Scene {
		private:
			dWorldID				world;
			dSpaceID				space;
			dJointGroupID			contactGroup;
			std::vector<Object *>	objects;

			friend void NearCallback(
				void *data,
				dGeomID o1,
				dGeomID o2
			);

		public:
			Scene();
			~Scene();

			void Update(
				real64_t dt
			);

			dWorldID GetWorld(
				void
			) const;

			dSpaceID GetSpace(
				void
			) const;
		} scene;

	} // namespace Physics

} // namespace XS
