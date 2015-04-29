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
			console.Print( PrintLevel::Normal, "Startup parameters:\n" );
			Indent indent( 1 );
			for ( const auto &arg : String::Split( &commandLine[start + 1], delimiter ) ) {
				console.Print( PrintLevel::Normal, "%s\n", arg.c_str() );
			}
		#endif
		}

		static void LoadConfig( void ) {
			const char *cfg = com_dedicated->GetBool() ? DEFAULT_CONFIG_SERVER : DEFAULT_CONFIG;
			const File f( cfg, FileMode::Read );

			if ( f.open ) {
				char *buffer = new char[f.length];
					f.Read( reinterpret_cast<uint8_t *>(buffer) );

					// normalise line endings
					for ( size_t i = 0; i < f.length; i++ ) {
						if ( buffer[i] == '\r' ) {
							buffer[i] = '\n';
						}
					}

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

			const File f( cfg, FileMode::Write );
			if ( !f.open ) {
				console.Print( PrintLevel::Normal, "Failed to write config! (%s)\n", cfg );
				return;
			}

			f.AppendString( str.c_str() );
		}

		static void Cmd_WriteConfig( const CommandContext * const context ) {
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
		srand( time( nullptr ) );

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

		XS::console.Print( XS::PrintLevel::Normal,
			WINDOW_TITLE " (" XSTR( ARCH_WIDTH ) " bits) built on " __DATE__ "\n"
		);

		if ( !XS::Common::com_dedicated->GetBool() ) {
			XS::Renderer::Init();
		}

		XS::Event::Init();
	//	XS::Network::Init();
		XS::Client::Input::Init();

		if ( XS::Common::com_developer->GetBool() ) {
			real64_t t = globalTimer.GetTiming( true, XS::TimerResolution::Milliseconds );
			XS::console.Print( XS::PrintLevel::Developer, "Init time: %.0f milliseconds\n", t );
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
			static real64_t currentTime = XS::Common::gameTimer->GetTiming( false, XS::TimerResolution::Milliseconds );
			static real64_t accumulator = 0.0;

			// calculate delta time for integrating this frame
			const real64_t newTime = XS::Common::gameTimer->GetTiming( false, XS::TimerResolution::Milliseconds );
			const real64_t dt = 1000.0 / XS::Common::com_framerate->GetReal64();
			const real64_t frameTime = newTime - currentTime;
			currentTime = newTime;

			real64_t sliceMsec = frameTime;
			if ( sliceMsec > 250.0 ) {
				sliceMsec = 250.0; // avoid spiral of death, maximum 250mspf
			}
			accumulator += sliceMsec;

			// input
			//TODO: run on another thread at 1000hz
			if ( !XS::Common::com_dedicated->GetBool() ) {
				XS::Client::Input::Poll();
			}

			// event pump
			XS::Event::Pump();
			XS::Command::ExecuteBuffer();

			// server frame, then network (snapshot)
		//	XS::Server::RunFrame( dt );
		//	XS::Server::NetworkPump();

			while ( accumulator >= dt ) {
				// this is so we can still react to input events if we're hitching

				// event pump
				XS::Event::Pump();
				XS::Command::ExecuteBuffer();

				// create movement command and send it off
				XS::Client::NetworkPump( dt );

				// run local prediction
				XS::Client::RunFrame( dt );
				accumulator -= dt;
			}

			// alpha = accumulator / dt;
			// lerp( previousState, alpha, currentState )
			XS::Client::DrawFrame( frameTime );
			XS::Renderer::Update( frameTime/*state*/ );

			const real64_t frameRate = XS::Common::r_framerate->GetReal64();
			const real64_t renderMsec = 1000.0 / frameRate;
			if ( frameTime < renderMsec ) {
				//TODO: busy-wait?
				//FIXME: retrieve scheduler granularity
				SDL_Delay( static_cast<uint32_t>( renderMsec - frameTime ) );
			}
		}
	}
	catch( const XS::XSError &e ) {
		const bool developer = XS::Common::com_developer->GetBool();

		XS::console.Print( XS::PrintLevel::Normal, "\n*** " PRODUCT_NAME " is shutting down\n" );
		if ( e.what() ) {
			XS::console.Print( XS::PrintLevel::Normal, "Reason: %s\n", e.what() );
		}
		XS::console.Print( XS::PrintLevel::Normal, "\n" );

		if ( developer ) {
			const real64_t runtime = globalTimer.GetTiming( true, XS::TimerResolution::Seconds );
			XS::console.Print( XS::PrintLevel::Developer, "Run time: %.3f seconds\n", runtime );
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
			const real64_t shutdownTime = globalTimer.GetTiming( false, XS::TimerResolution::Milliseconds );
			XS::console.Print( XS::PrintLevel::Normal, "Shutdown time: %.3f msec\n\n\n", shutdownTime );
		}

		return EXIT_SUCCESS;
	}

	// never reached
	return EXIT_FAILURE;
}
