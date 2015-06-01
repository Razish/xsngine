#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSVector.h"
#include "XSServer/XSServerGame.h"
#include "XSServer/XSEntity.h"
#include "XSServer/XSEntitySphere.h"

namespace XS {

	namespace ServerGame {

		GameState state = {};

		static void RegisterCvars( void ) {
		}

		static void Cmd_SpawnEntity( const CommandContext * const context ) {
			if ( context->size() != 3 ) {
				console.Print( PrintLevel::Normal,
					"usage: spawnentity <x> <y> <z>\n"
				);
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
			RegisterCvars();
			RegisterCommands();
		}

		void RunFrame( real64_t dt ) {
			for ( auto &entity : state.entities ) {
				entity->Update( dt );
			}
		}

		void GenerateSnapshot( void ) {
			//TODO: build a list of entity state changes
			for ( auto &entity : state.entities ) {
				size_t bufferSize = 0u;
				uint8_t *buffer = entity->Serialise( &bufferSize );
			}
		}

	} // namespace ServerGame

} // namespace XS
