#include "XSCommon/XSCommon.h"
#include "XSServer/XSServerGame.h"

namespace XS {

	namespace ServerGame {

		GameState state = {};

		static void RegisterCvars( void ) {
		}

		static void RegisterCommands( void ) {
		}

		void Init( void ) {
			RegisterCvars();
			RegisterCommands();
		}

		void Shutdown( void ) {
		}

		void RunFrame( real64_t dt ) {
			/*
			for ( auto &object : objects ) {
				object->Update( dt );
			}
			*/
		}

	} // namespace ServerGame

} // namespace XS
