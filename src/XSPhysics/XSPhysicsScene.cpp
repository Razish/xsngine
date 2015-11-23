#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSPhysics/XSPhysicsScene.h"
#include "XSCommon/XSVector.h"

namespace XS {

	namespace Physics {

		Scene scene;
		uint32_t Object::numObjects = 0;

		Object::Object() : id( numObjects++ ) {
			dWorldID world = scene.GetWorld();
			dSpaceID space = scene.GetSpace();

			// create the body
			body = dBodyCreate( world );

			// initialise mass
			dMassSetZero( &mass );
			dMassSetBoxTotal( &mass, 1.0, 1.0, 1.0, 1.0 );

			// set position
			dBodySetPosition( body, 0.0, 0.0, 0.0 );

			// set linear velocity
			dBodySetLinearVel( body, -1.0, 0.0, 0.0 );

			// no angular momentum for now

			// create the geom
			geom = dCreateBox( space, 1.0, 1.0, 1.0 );
			geomData = String::AllocMutable( String::Format( "Object %i", id ).c_str() );
			dGeomSetData( geom, static_cast<void *>( geomData ) );
			dGeomSetBody( geom, body );
		}

		Object::~Object() {
			delete[] geomData;
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

			Object *object = new Object();
			objects.push_back( object );
		}

		void NearCallback( void *data, dGeomID o1, dGeomID o2 ) {
			Scene *scene = static_cast<Scene *>( data );

			// get the dynamics body for each geom
			dBodyID b1 = dGeomGetBody( o1 );
			dBodyID b2 = dGeomGetBody( o2 );

			// create an array of dContact objects to hold the contact joints
			const size_t maxContacts = 32;
			dContact contacts[maxContacts];
			for ( dContact &contact : contacts ) {
				contact.surface.mode = dContactBounce | dContactSoftCFM;
				contact.surface.mu = dInfinity;
				contact.surface.mu2 = 0;
				contact.surface.bounce = 0.01;
				contact.surface.bounce_vel = 0.1;
				contact.surface.soft_cfm = 0.01;
			}

			// dCollide returns the number of actual contact points
			const int numContactPoints = dCollide( o1, o2, maxContacts, &contacts[0].geom, sizeof(dContact) );
			if ( numContactPoints ) {
				// add each contact point found to our joint group
				for ( int i = 0; i < numContactPoints; i++ ) {
					// dJointCreateContact returns a new dJointID which we then use with dJointAttach to finally create the temporary
					//	contact joint between the two geom bodies
					dJointID c = dJointCreateContact( scene->world, scene->contactGroup, contacts + i );
					dJointAttach( c, b1, b2 );
				}
			}
		}

		void Scene::Update( real64_t dt ) {
			dSpaceCollide( space, this, NearCallback );
			dWorldQuickStep( world, dt );
			dJointGroupEmpty( contactGroup );
		}

		Scene::~Scene() {
			for ( auto *object : objects ) {
				delete object;
			}
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
