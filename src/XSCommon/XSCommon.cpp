#include "XSSystem/XSInclude.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_keycode.h"

#include "XSSystem/XSPlatform.h"
#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSFile.h"
#include "XSClient/XSInput.h"
#include "XSClient/XSClient.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Common {

		static Cvar *com_dedicated;

		static void RegisterCvars( void ) {
			Cvar::Create( "com_date", __DATE__, CVAR_READONLY );
#ifdef DEDICATED
			com_dedicated = Cvar::Create( "com_dedicated", "1", CVAR_INIT );
#else
			com_dedicated = Cvar::Create( "com_dedicated", "0", CVAR_INIT );
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

			Cvar::Create( "com_path", cwd, CVAR_INIT );

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
			if ( start == std::string::npos )
				return;
			std::vector<std::string> args = String::Split( &commandLine[start+1], delim );

			Console::Print( "Startup parameters:\n" );
			Indent indent(1);
			for ( auto it = args.begin(); it != args.end(); ++it ) {
				Command::Append( it->c_str() );
				Console::Print( "%s\n", it->c_str() );
			}
		}

		static void Shutdown( const char *msg ) {
			Console::Print( "\n*** XSNGINE Shutdown: %s\n\n"
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
		XS::Console::Print( WINDOW_TITLE " built on " __DATE__ "\n" );

		// init
		XS::Command::Init(); // register commands like exec, vstr
		XS::Common::ParseCommandLine( argc, argv );

		XS::Common::RegisterCvars();
		XS::Cvar::LoadConfig();

		// now execute the command line args
		XS::Command::ExecuteBuffer();

		XS::File::Init();
		XS::Renderer::Init();

		XS::Event::Init();
	//	XS::Network::Init();

		XS::Console::Init();

		// frame
		while ( 1 ) {
			// input
			if ( !XS::Common::com_dedicated->GetBool() )
				XS::Client::input.Poll();

			// event pump
			XS::Event::Pump();
			XS::Command::ExecuteBuffer();

			// server frame, then network (snapshot)
		//	XS::Server::RunFrame();
		//	XS::Server::NetworkPump();

			// event pump
			XS::Event::Pump();
			XS::Command::ExecuteBuffer();

			// outgoing network (client command), then client frame
			XS::Client::NetworkPump();
			XS::Client::RunFrame();
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
