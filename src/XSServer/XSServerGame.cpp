#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSVector.h"
#include "XSNetwork/XSNetwork.h"
#include "XSPhysics/XSPhysicsActor.h"
#include "XSServer/XSServerGame.h"
#include "XSServer/XSEntity.h"
#include "XSServer/XSEntityFXRunner.h"
#include "XSServer/XSEntityModel.h"
#include "XSServer/XSServer.h"
#include "XSServer/XSResources.h"

namespace ServerGame {

	GameState svgState = {};

	void AddEntity( Entity *entity ) {
		auto it = std::find( svgState.entities.begin(), svgState.entities.end(), entity );
		if ( it == svgState.entities.end() ) {
			// not found
			svgState.entities.push_back( entity );
		}
		else {
			// already exists, this should not happen
			SDL_assert( !"Tried to add entity twice" );
			console.Print( PrintLevel::Normal, "Tried to add entity twice: %i\n",
				entity->id
			);
			return;
		}
	}

	void RemoveEntity( Entity *entity ) {
		auto it = std::find( svgState.entities.begin(), svgState.entities.end(), entity );
		delete *it;
		svgState.entities.erase( it );
	}

	static void Cmd_SpawnEntity( const CommandContext &context ) {
		if ( context.size() != 3 ) {
			console.Print( PrintLevel::Normal, "usage: spawnentity <x> <y> <z>\n" );
			return;
		}
		vector3 pos = {
			static_cast<real32_t>( std::atof( context[0].c_str() ) ),
			static_cast<real32_t>( std::atof( context[1].c_str() ) ),
			static_cast<real32_t>( std::atof( context[2].c_str() ) )
		};

#if 0
		EntityFXRunner *ent = new EntityFXRunner();
#else
		EntityModel *ent = new EntityModel();
#endif
		ent->position = pos;
		ent->physics.enabled = true;
		Physics::RigidActor *actor = new Physics::RigidActor( Physics::VolumeType::Sphere );
		actor->volume.sphere.radius = 2.0f;
		ent->physics.actor = actor;
		AddEntity( ent );
	}

	static void RegisterCommands( void ) {
		Command::AddCommand( "spawnentity", Cmd_SpawnEntity );
	}

	void Init( void ) {
		RegisterCommands();
	}

	void Shutdown( void ) {
		for ( auto &entity : svgState.entities ) {
			delete entity;
		}
		svgState.entities.clear();
	}

	void RunFrame( real64_t dt ) {
		for ( auto &entity : svgState.entities ) {
			entity->Update( dt );
		}
	}

} // namespace ServerGame
