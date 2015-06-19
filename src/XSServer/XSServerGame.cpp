#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSVector.h"
#include "XSServer/XSServerGame.h"
#include "XSServer/XSEntity.h"
#include "XSServer/XSEntitySphere.h"
#include "XSServer/XSServer.h"

namespace XS {

	namespace ServerGame {

		GameState state = {};

		static void Cmd_SpawnEntity( const CommandContext * const context ) {
			if ( context->size() != 3 ) {
				console.Print( PrintLevel::Normal, "usage: spawnentity <x> <y> <z>\n" );
				return;
			}
			vector3 pos(
				std::atof( (*context)[0].c_str() ),
				std::atof( (*context)[1].c_str() ),
				std::atof( (*context)[2].c_str() )
			);

			EntitySphere *ent = new EntitySphere();

			ent->position = pos;
		}

		static void RegisterCommands( void ) {
			Command::AddCommand( "spawnentity", Cmd_SpawnEntity );
		}

		void Init( void ) {
			// reserve 1024 entities for now, test contiguity
			state.entities.reserve( 1024u );

			RegisterCommands();
		}

		void RunFrame( real64_t dt ) {
			for ( auto &entity : state.entities ) {
				entity->Update( dt );
			}
		}

	} // namespace ServerGame

} // namespace XS
