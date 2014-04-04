#include "XSSystem/XSInclude.h"
#include "XSSystem/XSPlatform.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_keycode.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSTimer.h"
#include "XSClient/XSInput.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSKeys.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderer.h"
#include "XSSystem/XSOS.h"

namespace XS {

	namespace Common {

		#define DEFAULT_CONFIG			"cfg/xsn.cfg"
		#define DEFAULT_CONFIG_SERVER	"cfg/xsn_server.cfg"

		Cvar *com_dedicated, *com_developer, *com_path;
		static Cvar *com_framerate;

		static void RegisterCvars( void ) {
			Cvar::Create( "com_date", __DATE__, CVAR_READONLY );
			com_dedicated = Cvar::Create( "com_dedicated", "0", CVAR_INIT );
#ifdef _DEBUG
			com_developer = Cvar::Create( "com_developer", "1", CVAR_NONE );
#else
			com_developer = Cvar::Create( "com_developer", "0", CVAR_NONE );
#endif
			com_framerate = Cvar::Create( "com_framerate", "120", CVAR_NONE );
			com_path = Cvar::Get( "com_path" );
		}

		static void ParseCommandLine( int argc, char **argv ) {
			std::string commandLine;
			static char cwd[FILENAME_MAX];

			OS::GetCurrentWorkingDirectory( cwd, sizeof(cwd) );
			com_path->Set( cwd );

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
			for ( const auto &it : args ) {
				Command::Append( it.c_str() );
				Console::Print( "%s\n", it.c_str() );
			}
		}

		static void LoadConfig( void ) {
			const char *cfg = com_dedicated->GetBool() ? DEFAULT_CONFIG_SERVER : DEFAULT_CONFIG;
			const File f( cfg, FM_READ );

			if ( f.open ) {
				char *buffer = new char[f.length];
					f.Read( (byte *)buffer );

					Command::ExecuteBuffer(); // flush buffer before we issue commands
					char *current = strtok( buffer, "\n" );
					while ( current ) {
						Command::Append( current );
						Command::ExecuteBuffer();
						current = strtok( NULL, "\n" );
					}
				delete[] buffer;
			}

			Cvar::initialised = true;
		}

		static void WriteConfig( const char *cfg = NULL ) {
			std::string str = "";
			Cvar::WriteCvars( str );
			if ( !com_dedicated->GetBool() )
				Client::WriteBinds( str );

			// default config if none specified
			if ( !cfg ) {
				cfg = com_dedicated->GetBool() ? DEFAULT_CONFIG_SERVER : DEFAULT_CONFIG;
			}

			const File f( cfg, FM_WRITE );
			if ( !f.open ) {
				Console::Print( "Failed to write config! (%s)\n", cfg );
				return;
			}

			f.AppendString( str.c_str() );
		}

		static void Cmd_WriteConfig( const commandContext_t *context ) {
			const char *cfg = NULL;
			if ( context->size() )
				 cfg = (*context)[0].c_str();

			WriteConfig( cfg );
		}

	} // namespace Common

} // namespace XS

static XS::Timer timer;

int main( int argc, char **argv ) {
	try {
		XS::Console::Print( WINDOW_TITLE " (" XSTR( ARCH_WIDTH ) " bits) built on " __DATE__ " [git " REVISION "]\n" );

		// init
		XS::File::Init();
		XS::Command::Init(); // register commands like exec, vstr
		XS::Command::AddCommand( "writeconfig", XS::Common::Cmd_WriteConfig );
		XS::Common::ParseCommandLine( argc, argv );

		//
		// DO NOT LOAD MEDIA BEFORE THIS POINT
		//

		XS::Common::RegisterCvars();
		// execute the command line args, so config can be loaded from an overridden com_path
		XS::Command::ExecuteBuffer();
		XS::Common::LoadConfig();

		// base path should be finalised by now
		XS::Console::Print( "Base path: %s\n", XS::Common::com_path->GetCString() );

		if ( !XS::Common::com_dedicated->GetBool() )
			XS::Renderer::Init();

		XS::Console::Init();

		XS::Event::Init();
	//	XS::Network::Init();

		if ( XS::Common::com_developer->GetBool() ) {
			double t = timer.GetTiming( true, XS::Timer::MILLISECONDS );
			XS::Console::Print( "Init time: %.0f milliseconds\n", (float)t );
		}

		if ( !XS::Common::com_dedicated->GetBool() ) {
			XS::Client::Init();
		}

		// frame
		XS::Timer gameTimer;
		while ( 1 ) {
			static double currentTime = gameTimer.GetTiming();
			static double accumulator = 0.0;

			// calculate delta time for integrating this frame
			double newTime = gameTimer.GetTiming();
			double frametime = newTime - currentTime;
			double dt = 1.0 / XS::Common::com_framerate->GetDouble();
			if ( frametime > 1.0 / 4.0 )
				frametime = 1.0 / 4.0; // avoid spiral of death, maximum 250mspf
			currentTime = newTime;
			accumulator += frametime;

			// input
			if ( !XS::Common::com_dedicated->GetBool() )
				XS::Client::input.Poll();

			// event pump
			XS::Event::Pump();
			XS::Command::ExecuteBuffer();

			// server frame, then network (snapshot)
		//	XS::Server::RunFrame( dt );
		//	XS::Server::NetworkPump();

			// event pump
			XS::Event::Pump();
			XS::Command::ExecuteBuffer();

			// outgoing network (client command), then client frame
			XS::Client::NetworkPump();
			while ( accumulator >= dt ) {
				XS::Client::RunFrame( dt );
				accumulator -= dt;
			}

		//	const double alpha = accumulator / dt;
			// lerp( previousState, alpha, currentState )
			XS::Client::DrawFrame();
			XS::Renderer::Update( /*state*/ );
		}
	}
	catch( const XS::XSError &e ) {
		bool developer = XS::Common::com_developer->GetBool();

		XS::Console::Print( "\n*** XSNGINE Shutdown: %s\n\n"
			"Cleaning up...\n", e.what() );

		if ( developer ) {
			double t = timer.GetTiming( true, XS::Timer::MILLISECONDS );
			XS::Console::Print( "Run time: %.0f milliseconds\n", (float)t );
		}

		// indent the console for this scope
		{
			XS::Indent indent(1);
			XS::Client::Shutdown();
			XS::Renderer::Shutdown();

			XS::Common::WriteConfig();
			XS::Cvar::Clean();
		}

		if ( developer ) {
			double t = timer.GetTiming( false, XS::Timer::MILLISECONDS );
			XS::Console::Print( "Shutdown time: %.0f milliseconds\n", (float)t );
		}

		XS::Console::Close();

		return EXIT_SUCCESS;
	}

	// never reached
	return EXIT_FAILURE;
}
