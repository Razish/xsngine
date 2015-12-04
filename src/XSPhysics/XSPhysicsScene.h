#pragma once

#include <ode/ode.h>
#include <vector>

#include "XSCommon/XSVector.h"

namespace XS {

	namespace Physics {

		// uses the same coordinate system as the renderer (i.e. +Y is up)

		class Object {

		private:
			static uint32_t numObjects;
			class Scene	*scene = nullptr;
			dBodyID		 body;
			dGeomID		 geom;
			dMass		 mass;

		public:
			// don't allow default instantiation
			Object() = delete;
			Object( const Object& ) = delete;
			Object& operator=( const Object& ) = delete;

			Object( Scene *physicsScene );
			~Object();

			char			*geomData = nullptr;
			uint32_t		id;

			// set the object's position
			void SetPosition(
				const vector3 *pos
			) const;

			// get the object's position
			vector3 GetPosition(
				void
			) const;

		};

		class Scene {

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

			void AddObject(
				Object *object
			);

			// called by Object destructor
			void RemoveObject(
				Object *object
			);

		};

	} // namespace Physics

} // namespace XS
