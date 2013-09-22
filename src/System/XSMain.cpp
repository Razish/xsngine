#include "System/XSInclude.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_keycode.h"

#include "XSCommon/XSCvar.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSClient/XSInput.h"
#include "XSRenderer/XSRenderer.h"

using namespace XS;

static Cvar *com_dedicated;

static void RegisterCvars( void ) {
					Cvar::Create( "com_date", __DATE__, Cvar::READONLY );
#ifdef DEDICATED
	com_dedicated = Cvar::Create( "com_dedicated", "1", Cvar::INIT );
#else
	com_dedicated = Cvar::Create( "com_dedicated", "0", Cvar::INIT );
#endif
}

static void ParseCommandLine( int argc, char **argv ) {
	char commandLine[1024] = {0};
	static char cwd[FILENAME_MAX];

#ifdef _WIN32
	_getcwd( cwd, sizeof( cwd )-1 );
#else
	getcwd( cwd, sizeof( cwd )-1 );
#endif
	cwd[FILENAME_MAX-1] = '\0';

	Cvar::Create( "com_path", cwd, Cvar::INIT );

	for ( int i=1; i<argc; i++ ) {
		const bool containsSpaces = strchr( argv[i], ' ' ) != NULL;

		if ( containsSpaces )
			String::Concatenate( commandLine, sizeof( commandLine ), "\"" );

		String::Concatenate( commandLine, sizeof( commandLine ), argv[i] );

		if ( containsSpaces )
			String::Concatenate( commandLine, sizeof( commandLine ), "\"" );

		String::Concatenate( commandLine, sizeof( commandLine ), " " );
	}

	bool inQuotes = false;
	std::vector<char*> args;

	for ( char *p=commandLine; *p; p++ ) {
		if ( *p == '"' )
			inQuotes = !inQuotes;

		// look for a + separating character
		if ( (*p == '+' && !inQuotes) || *p == '\n' || *p == '\r' ) {
			args.push_back( p+1 );
			*p = '\0';
		}

		p++;
	}
}

static void Shutdown( const char *msg ) {
	Print( "\n*** XSNGINE Shutdown: %s\n\n"
			"Cleaning up...\n", msg );

	// indent the console for this scope
	Indent indent(1);
		Renderer::Shutdown();
		// shutdown
		Cvar::WriteConfig();
		Cvar::Clean();
}

int main( int argc, char **argv ) {
	try {
		// init
	//	Command::Init(); // register commands like exec, vstr
		ParseCommandLine( argc, argv );

		RegisterCvars();

		Cvar::LoadConfig();
	//	FileSystem::Init(); // probably don't need. maybe. yet.
		Renderer::Init();

		Event::Init();
	//	Network::Init();

		// frame
		while ( 1 ) {
			// input
			if ( !com_dedicated->GetBool() )
				Client::input.Poll();

			// event pump
			Event::Pump();
		//	Command::ExecuteBuffer();

			// server frame, then network (snapshot)
		//	Server::RunFrame();
		//	Server::NetworkPump();

			// event pump
			Event::Pump();
		//	Command::ExecuteBuffer();

			// outgoing network (client command), then client frame
		//	Client::NetworkPump();
		//	Client::RunFrame();
			Renderer::Update();
		}
	}
	catch( const char *msg ) {
		Shutdown( msg );
		return EXIT_SUCCESS;
	}
	catch( std::string &msg ) {
		Shutdown( msg.c_str() );
		return EXIT_SUCCESS;
	}

	// never reached
	return EXIT_FAILURE;
}
