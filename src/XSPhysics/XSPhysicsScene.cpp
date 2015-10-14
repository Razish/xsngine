#include "XSCommon/XSCommon.h"
#include "XSPhysics/XSPhysicsScene.h"
#include "XSCommon/XSVector.h"

namespace XS {

	namespace Physics {

		Scene scene;

		Object::Object() {
			dWorldID world = scene.GetWorld();
			dSpaceID space = scene.GetSpace();

			body = dBodyCreate( world );
			dBodySetPosition( body, 0.0, 10.0, 0.0 );
			dBodySetLinearVel( body, 0.0, 0.0, 0.0 );
			dMatrix3 R;
			dRFromAxisAndAngle( R,
				dRandReal() * 2.0 - 1.0,
				dRandReal() * 2.0 - 1.0,
				dRandReal() * 2.0 - 1.0,
				dRandReal() * 10.0 - 5.0
			);
			dBodySetRotation( body, R );
			dMass m;
			const real64_t density = 0.5;
			dReal sides[3] = { 2.0, 2.0, 2.0 };
			dMassSetBox( &m, density, sides[0], sides[1], sides[2] );
			dBodySetMass( body, &m );
			geom = dCreateBox( space, sides[0], sides[1], sides[2] );
			dGeomSetBody( geom, body );
		}

		Scene::Scene() {
			world = dWorldCreate();
			dWorldSetERP( world, 0.2 );
			dWorldSetCFM( world, 1e-5 );
			dWorldSetGravity( world, 0.0, -1.0, 0.0 );
			dWorldSetContactMaxCorrectingVel( world, 0.9 );
			dWorldSetContactSurfaceLayer( world, 0.00001 ); //FIXME: find a nice epsilon
			dWorldSetAutoDisableFlag( world, 1 ); // automatically disable "at rest" objects

			space = dSimpleSpaceCreate( 0 );
			contactGroup = dJointGroupCreate( 0 );

			dCreatePlane( space, 0.0, 1.0, 0.0, 0.0 );

			Object object;
			objects.push_back( object );
		}

		Scene::~Scene() {
			dJointGroupDestroy( contactGroup );
			dSpaceDestroy( space );
			dWorldDestroy( world );
		}

		dWorldID Scene::GetWorld( void ) const {
			return world;
		};

		dSpaceID Scene::GetSpace( void ) const {
			return space;
		};

	} // namespace Physics

} // namespace XS
