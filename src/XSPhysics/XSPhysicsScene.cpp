#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSPhysics/XSPhysicsScene.h"
#include "XSCommon/XSVector.h"

namespace XS {

	namespace Physics {

		uint32_t Object::numObjects = 0;

		Object::Object( Scene *physicsScene ) : id( numObjects++ ) {
			SDL_assert( physicsScene );
			scene = physicsScene;

			dWorldID world = scene->GetWorld();
			dSpaceID space = scene->GetSpace();

			// create the body
			body = dBodyCreate( world );

			// initialise mass
			dMassSetZero( &mass );
			dMassSetBox( &mass, 1.0, 1.0, 1.0, 1.0 );

			// set linear velocity
			//dBodySetLinearVel( body, -1.0, 0.0, 0.0 );

			// no angular momentum for now

			// create the geom
			geom = dCreateBox( space, 1.0, 1.0, 1.0 );
			geomData = String::AllocMutable( String::Format( "Object %i", id ).c_str() );
			dGeomSetData( geom, geomData );
			dGeomSetBody( geom, body );
		}

		Object::~Object() {
			SDL_assert( scene );
			scene->RemoveObject( this );
			delete[] geomData;
		}

		void Object::SetPosition( const vector3 *pos ) const {
			const dReal x = pos->data[0];
			const dReal y = pos->data[1];
			const dReal z = pos->data[2];
			dBodySetPosition( body, x, y, z );
			dGeomSetPosition( geom, x, y, z );
		}

		vector3 Object::GetPosition( void ) const {
			const dReal *pos = dGeomGetPosition( geom );
			vector3 v;
			v[0] = pos[0];
			v[1] = pos[1];
			v[2] = pos[2];
			console.Print( PrintLevel::Normal, "%s\n", v.tostring().c_str() );
			return v;
		}

		Scene::Scene() {
			dInitODE();

			world = dWorldCreate();
			dWorldSetERP( world, 0.2 );
			dWorldSetCFM( world, 1e-5 );
			dWorldSetGravity( world, 0.0, -9.81, 0.0 );
			dWorldSetContactMaxCorrectingVel( world, 0.9 );
			dWorldSetContactSurfaceLayer( world, 0.00001 ); //FIXME: find a nice epsilon
			dWorldSetAutoDisableFlag( world, 1 ); // automatically disable "at rest" objects

			space = dSimpleSpaceCreate( 0 );
			contactGroup = dJointGroupCreate( 0 );

			dGeomID planeID = dCreatePlane( space, 0.0, 1.0, 0.0, 0.0 );
			dGeomSetData( planeID, String::AllocMutable( "test plane" ) );
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
					//console.Print( PrintLevel::Normal, "contact between \"%s\" and \"%s\"\n",
					//	(const char *)dGeomGetData( o1 ), (const char *)dGeomGetData( o2 )
					//);
					dJointID c = dJointCreateContact( scene->world, scene->contactGroup, contacts + i );
					dJointAttach( c, b1, b2 );
				}
			}
		}

		void Scene::Update( real64_t dt ) {
			if ( dt <= 0.0 ) {
				return;
			}
			dSpaceCollide( space, this, NearCallback );
			if ( !dWorldQuickStep( world, (dt / 1000.0) ) ) {
				console.Print( PrintLevel::Normal, "physics step failed\n" );
			}
			dJointGroupEmpty( contactGroup );
		}

		Scene::~Scene() {
			//TODO: verify we have a clean state, all objects etc should have been removed by now

			for ( auto *object : objects ) {
				delete object;
			}

			dJointGroupDestroy( contactGroup );
			dSpaceDestroy( space );
			dWorldDestroy( world );
			dCloseODE();
		}

		dWorldID Scene::GetWorld( void ) const {
			return world;
		};

		dSpaceID Scene::GetSpace( void ) const {
			return space;
		};

		void Scene::AddObject( Object *object ) {
			objects.push_back( object );
		}

		void Scene::RemoveObject( Object *object ) {
			auto it = std::find( objects.begin(), objects.end(), object );
			if ( it != objects.end() ) {
				objects.erase( it );
				console.Print( PrintLevel::Developer, "Removing \"%s\" from physics objects\n",
					object->geomData
				);
			}
			else {
				console.Print( PrintLevel::Developer, "Tried to remove non-existent physics object \"%s\"\n",
					object->geomData
				);
			}
		}

	} // namespace Physics

} // namespace XS
