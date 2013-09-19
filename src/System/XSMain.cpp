#include <stdlib.h>
#include <stdio.h>

#include "XSCommon/XSCvar.h"
#include "XSCommon/XSEvent.h"
#include "XSClient/XSInput.h"
#include "XSClient/XSVideo.h"

using namespace XS;

Cvar *com_dedicated = NULL;
static void RegisterCvars( void ) {
	com_dedicated = Cvar::Create( "com_dedicated", "0", Cvar::INIT );
}

int main( int argc, char **argv ) {
	// init
//	Command::Init(); // register commands like exec, vstr
//	Command::ParseCommandLine( argc, argv );

	RegisterCvars();

	Cvar::LoadConfig();
//	FileSystem::Init(); // probably don't need. maybe. yet.
	Client::video.Init();

//	Network::Init();

	Cvar::LockInit();

	// frame
	try {
		while ( 1 ) {
			// input
			if ( !com_dedicated->Bool() )
				Client::input.Poll();

			// event pump
			Common::EventPump();
		//	Command::ExecuteBuffer();

			// server frame, then network (snapshot)
		//	Server::RunFrame();
		//	Server::NetworkPump();

			// event pump
			Common::EventPump();
		//	Command::ExecuteBuffer();

			// outgoing network (client command), then client frame
		//	Client::NetworkPump();
		//	Client::RunFrame();
			Client::video.Update();
		}
	}
	catch( const char *msg ) {
		Print( "XSNGINE Shutdown: %s\n", msg );

		// shutdown
		Cvar::WriteConfig();

		return 0;
	}

	// never reached
	return 1;
}
