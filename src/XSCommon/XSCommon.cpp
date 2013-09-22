#include "XSSystem/XSInclude.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_keycode.h"

#include "XSCommon/XSCvar.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSClient/XSInput.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Common {

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
			std::string commandLine;
			static char cwd[FILENAME_MAX];

#ifdef _WIN32
			_getcwd( cwd, sizeof( cwd )-1 );
#else
			getcwd( cwd, sizeof( cwd )-1 );
#endif
			cwd[FILENAME_MAX-1] = '\0';

			Cvar::Create( "com_path", cwd, Cvar::INIT );

			// concatenate argv[] to commandLine
			for ( int i=1; i<argc; i++ ) {
				const bool containsSpaces = strchr( argv[i], ' ' ) != NULL;

				if ( containsSpaces )
					commandLine += "\"";

				commandLine += argv[i];

				if ( containsSpaces )
					commandLine += "\"";

				commandLine += " ";
			}

			// split up commandLine by +
			//	+set x y +set herp derp
			// becomes
			//	set x y
			//	set herp derp
			// then append it to the command buffer

			const char delim = '+';
			size_t start = commandLine.find( delim );
			std::vector<std::string> args = String::Split( &commandLine[start+1], delim );
			for ( auto it = args.begin(); it != args.end(); ++it ) {
			//	Command::Append( it->c_str() );
			}
		//	Command::ExecuteBuffer();
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

	} // namespace Common

} // namespace XS

int main( int argc, char **argv ) {
	try {
		// init
	//	XS::Command::Init(); // register commands like exec, vstr
		XS::Common::ParseCommandLine( argc, argv );

		XS::Common::RegisterCvars();

		XS::Cvar::LoadConfig();
	//	XS::FileSystem::Init(); // probably don't need. maybe. yet.
		XS::Renderer::Init();

		XS::Event::Init();
	//	XS::Network::Init();

		// frame
		while ( 1 ) {
			// input
			if ( !XS::Common::com_dedicated->GetBool() )
				XS::Client::input.Poll();

			// event pump
			XS::Event::Pump();
		//	XS::Command::ExecuteBuffer();

			// server frame, then network (snapshot)
		//	XS::Server::RunFrame();
		//	XS::Server::NetworkPump();

			// event pump
			XS::Event::Pump();
		//	XS::Command::ExecuteBuffer();

			// outgoing network (client command), then client frame
		//	XS::Client::NetworkPump();
		//	XS::Client::RunFrame();
			XS::Renderer::Update();
		}
	}
	catch( const char *msg ) {
		XS::Common::Shutdown( msg );
		return EXIT_SUCCESS;
	}
	catch( std::string &msg ) {
		XS::Common::Shutdown( msg.c_str() );
		return EXIT_SUCCESS;
	}

	// never reached
	return EXIT_FAILURE;
}
