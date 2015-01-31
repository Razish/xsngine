#include <SDL2/SDL_timer.h>
#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSTimer.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSGlobals.h"
#include "XSClient/XSClient.h"
#include "XSInput/XSInput.h"
#include "XSInput/XSKeys.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Common {

		#define DEFAULT_CONFIG			"cfg/xsn.cfg"
		#define DEFAULT_CONFIG_SERVER	"cfg/xsn_server.cfg"

		Cvar *com_dedicated;
		Cvar *com_developer;
		static Cvar *com_framerate;
		static Cvar *r_framerate;

		static void RegisterCvars( void ) {
			Cvar::Create( "com_date", __DATE__, "Compilation date", CVAR_READONLY );
			com_dedicated = Cvar::Create( "com_dedicated", "0", "True if running a dedicated server", CVAR_INIT );
		#ifdef _DEBUG
			com_developer = Cvar::Create( "com_developer", "1", "Developer mode", CVAR_NONE );
		#else
			com_developer = Cvar::Create( "com_developer", "0", "Developer mode", CVAR_NONE );
		#endif // _DEBUG
			com_framerate = Cvar::Create( "com_framerate", "50", "Game tick rate", CVAR_ARCHIVE );
			r_framerate = Cvar::Create( "r_framerate", "120", "Render framerate", CVAR_ARCHIVE );
		}

		static void ParseCommandLine( int32_t argc, char **argv ) {
			std::string commandLine;

			// concatenate argv[] to commandLine
			for ( int32_t i = 1; i < argc; i++ ) {
				const bool containsSpaces = strchr( argv[i], ' ' ) != nullptr;

				if ( containsSpaces ) {
					commandLine += "\"";
				}

				commandLine += argv[i];

				if ( containsSpaces ) {
					commandLine += "\"";
				}

				commandLine += " ";
			}

			// split up commandLine by +
			//	+set x y +set herp derp
			// becomes
			//	set x y
			//	set herp derp
			// then append it to the command buffer
			const char delimiter = '+';
			const size_t start = commandLine.find( delimiter );
			if ( start == std::string::npos ) {
				return;
			}
			Command::Append( &commandLine[start + 1], delimiter );

		#ifdef _DEBUG
			console.Print( "Startup parameters:\n" );
			Indent indent( 1 );
			for ( const auto &arg : String::Split( &commandLine[start + 1], delimiter ) ) {
				console.Print( "%s\n", arg.c_str() );
			}
		#endif
		}

		static void LoadConfig( void ) {
			const char *cfg = com_dedicated->GetBool() ? DEFAULT_CONFIG_SERVER : DEFAULT_CONFIG;
			const File f( cfg, FileMode::READ );

			if ( f.open ) {
				char *buffer = new char[f.length];
					f.Read( reinterpret_cast<uint8_t *>(buffer) );

					Command::ExecuteBuffer(); // flush buffer before we issue commands
					char *current = strtok( buffer, "\n" );
					while ( current ) {
						Command::Append( current );
						Command::ExecuteBuffer();
						current = strtok( nullptr, "\n" );
					}
				delete[] buffer;
			}
		}

		static void WriteConfig( const char *cfg = nullptr ) {
			std::string str = "";
			Cvar::WriteCvars( str );
			if ( !com_dedicated->GetBool() ) {
				Client::WriteBinds( str );
			}

			// default config if none specified
			if ( !cfg ) {
				cfg = com_dedicated->GetBool() ? DEFAULT_CONFIG_SERVER : DEFAULT_CONFIG;
			}

			const File f( cfg, FileMode::WRITE );
			if ( !f.open ) {
				console.Print( "Failed to write config! (%s)\n", cfg );
				return;
			}

			f.AppendString( str.c_str() );
		}

		static void Cmd_WriteConfig( const commandContext_t * const context ) {
			const char *cfg = nullptr;
			if ( context->size() ) {
				 cfg = (*context)[0].c_str();
			 }

			WriteConfig( cfg );
		}

		Timer *gameTimer = nullptr;

	} // namespace Common

} // namespace XS

static XS::Timer globalTimer;

int main( int argc, char **argv ) {
	try {
		// critical initialisation
		XS::File::Init();
		XS::Common::RegisterCvars();
		XS::Command::Init(); // register commands like exec, vstr
		XS::Command::AddCommand( "writeconfig", XS::Common::Cmd_WriteConfig );
		XS::Common::ParseCommandLine( argc, argv );

		// execute the command line args, so config can be loaded from an overridden base path
		XS::Command::ExecuteBuffer();
		XS::File::SetBasePath();
		XS::Common::LoadConfig();

		//
		// DO NOT LOAD MEDIA BEFORE THIS POINT
		//

		XS::console.Print( WINDOW_TITLE " (" XSTR( ARCH_WIDTH ) " bits) built on " __DATE__ "\n" );

		if ( !XS::Common::com_dedicated->GetBool() ) {
			XS::Renderer::Init();
		}

		XS::Event::Init();
	//	XS::Network::Init();

		if ( XS::Common::com_developer->GetBool() ) {
			real64_t t = globalTimer.GetTiming( true, XS::Timer::Resolution::MILLISECONDS );
			XS::console.DebugPrint( "Init time: %.0f milliseconds\n", t );
		}

		if ( !XS::Common::com_dedicated->GetBool() ) {
			XS::Client::Init();
		}

		// post-init stuff
		// can no longer modify cvars flagged with CVAR_INIT
		XS::Cvar::initialised = true;

		// frame
		XS::Common::gameTimer = new XS::Timer(); // TODO: free
		while ( 1 ) {
			static real64_t currentTime = XS::Common::gameTimer->GetTiming( false,
				XS::Timer::Resolution::MILLISECONDS );
			static real64_t accumulator = 0.0;

			// calculate delta time for integrating this frame
			const real64_t newTime = XS::Common::gameTimer->GetTiming( false, XS::Timer::Resolution::MILLISECONDS );
			const real64_t dt = 1000.0 / XS::Common::com_framerate->GetDouble();
			const real64_t frameTime = newTime - currentTime;
			currentTime = newTime;

			real64_t sliceMsec = frameTime;
			if ( sliceMsec > 250.0 ) {
				sliceMsec = 250.0; // avoid spiral of death, maximum 250mspf
			}
			accumulator += sliceMsec;

			// input
			if ( !XS::Common::com_dedicated->GetBool() ) {
				XS::Client::input.Poll();
			}

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

			// alpha = accumulator / dt;
			// lerp( previousState, alpha, currentState )
			XS::Client::DrawFrame( frameTime );
			XS::Renderer::Update( /*state*/ );

			const real64_t frameRate = XS::Common::r_framerate->GetDouble();
			const real64_t renderMsec = 1000.0 / frameRate;
			if ( frameTime < renderMsec ) {
				XS::console.DebugPrint( "frameTime %.5f < %.5f, delaying for %0i\n", frameTime, renderMsec,
					(uint32_t)(renderMsec - frameTime) );
				SDL_Delay( (uint32_t)(renderMsec - frameTime) );
			}
		}
	}
	catch( const XS::XSError &e ) {
		const bool developer = XS::Common::com_developer->GetBool();

		XS::console.Print( "\n*** xsngine is shutting down\nReason: %s\n\n", e.what() );

		if ( developer ) {
			const real64_t runtime = globalTimer.GetTiming( true, XS::Timer::Resolution::SECONDS );
			XS::console.DebugPrint( "Run time: %.3f seconds\n", runtime );
		}

		// indent the console for this scope
		{
			XS::Indent indent( 1 );
			XS::Client::Shutdown();
			XS::Renderer::Shutdown();

			if ( XS::Cvar::initialised ) {
				// only write out the configuration if xsngine was able to fully initialise, else we'll be writing
				//	default values
				XS::Common::WriteConfig();
				XS::Cvar::Clean();
			}
		}

		if ( developer ) {
			const real64_t shutdownTIme = globalTimer.GetTiming( false, XS::Timer::Resolution::SECONDS );
			XS::console.DebugPrint( "Shutdown time: %.3f seconds\n\n\n", shutdownTIme );
		}

		return EXIT_SUCCESS;
	}

	// never reached
	return EXIT_FAILURE;
}
