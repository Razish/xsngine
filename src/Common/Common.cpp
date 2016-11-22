#include <SDL2/SDL_timer.h>
#include <SDL2/SDL.h>

#include "Common/Common.h"
#include "Common/Command.h"
#include "Common/Console.h"
#include "Common/Cvar.h"
#include "Common/Event.h"
#include "Common/Error.h"
#include "Common/File.h"
#include "Common/Timer.h"
#include "Common/String.h"
#include "Common/Globals.h"
#include "Client/Client.h"
#include "Input/Input.h"
#include "Input/Keys.h"
#include "Network/Network.h"
#include "Renderer/Renderer.h"
#include "Server/Server.h"

namespace Common {

	#define DEFAULT_CONFIG			"cfg/xsn.cfg"
	#define DEFAULT_CONFIG_SERVER	"cfg/xsn_server.cfg"

	static Cvar *com_busyWait = nullptr;
	Cvar *com_dedicated = nullptr;
	Cvar *com_developer = nullptr;
	static Cvar *com_framerate = nullptr;
	static Cvar *com_profile = nullptr;
	static Cvar *r_framerate = nullptr;

	static void RegisterCvars( void ) {
		const Cvar * XS_UNUSED dummyCvar;
		dummyCvar = Cvar::Create( "com_date", __DATE__,
			"Compilation date", CVAR_READONLY
		);
		com_busyWait = Cvar::Create( "com_busyWait", "0",
			"Regulate FPS by spinning in a loop", CVAR_ARCHIVE
		);
		com_dedicated = Cvar::Create( "com_dedicated", "0",
			"True if running a dedicated server", CVAR_INIT
		);
		com_developer = Cvar::Create( "com_developer", XS_DEBUG_BUILD ? "1" : "0",
			"Developer mode", CVAR_NONE
		);
		com_framerate = Cvar::Create( "com_framerate", "10",
			"Game tick rate", CVAR_ARCHIVE
		);
		com_profile = Cvar::Create( "com_profile", "0",
			"Print timing statistics", CVAR_NONE
		);
		dummyCvar = Cvar::Create( "com_revision", XS_REVISION,
			"git revision", CVAR_READONLY
		);
		r_framerate = Cvar::Create( "r_framerate", "120",
			"Render framerate", CVAR_ARCHIVE
		);
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
		std::vector<std::string> args;
		String::Split( &commandLine[start + 1], delimiter, args );
		for ( const auto &arg : args ) {
			console.Print( PrintLevel::Normal, "%s\n", arg.c_str() );
		}
	#endif
	}

	static void LoadConfig( void ) {
		const char *cfg = com_dedicated->GetBool() ? DEFAULT_CONFIG_SERVER : DEFAULT_CONFIG;
		const File f( cfg, FileMode::Read );

		if ( f.isOpen ) {
			char *buffer = new char[f.length];
				f.Read( reinterpret_cast<uint8_t *>(buffer) );

				// normalise line endings
				for ( long i = 0; i < f.length; i++ ) {
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
		const bool dedicated = com_dedicated->GetBool();
		if ( !dedicated ) {
			Client::WriteBinds( str );
		}

		// default config if none specified
		if ( !cfg ) {
			cfg = dedicated ? DEFAULT_CONFIG_SERVER : DEFAULT_CONFIG;
		}

		const File f( cfg, FileMode::Write );
		if ( !f.isOpen ) {
			console.Print( PrintLevel::Normal, "Failed to write config! (%s)\n", cfg );
			return;
		}

		f.AppendString( str.c_str() );
	}

	static void Cmd_WriteConfig( const CommandContext &context ) {
		const char *cfg = nullptr;
		if ( context.size() ) {
			 cfg = context[0].c_str();
		 }

		WriteConfig( cfg );
	}

	Timer *gameTimer = nullptr;

} // namespace Common

static Timer globalTimer;

//FIXME: function try block
int main( int argc, char **argv ) try {
	srand( time( nullptr ) );

	// critical initialisation
	File::Init();
	Common::RegisterCvars();
	Command::Init(); // register commands like exec, vstr
	Command::AddCommand( "writeconfig", Common::Cmd_WriteConfig );
	Common::ParseCommandLine( argc, argv );

	// execute the command line args, so config can be loaded from an overridden base path
	Command::ExecuteBuffer();
	File::SetBasePath();
	Common::LoadConfig();

	const bool dedicated = Common::com_dedicated->GetBool();

	//
	// DO NOT LOAD MEDIA BEFORE THIS POINT
	//

	console.Print( PrintLevel::Normal,
		WINDOW_TITLE " (" XSTR( ARCH_WIDTH ) " bits) built on " __DATE__ " with " XS_COMPILER "\n"
	);

	if ( !dedicated ) {
		Renderer::Init();
	}

	Event::Init();
	if ( dedicated ) {
		Server::Init();
	}
	else {
		Client::Input::Init();
		Client::Init();
	}

	Network::Init();

	if ( Common::com_profile->GetBool() ) {
		real64_t t = globalTimer.GetTiming( true, TimerResolution::Milliseconds );
		console.Print( PrintLevel::Normal, "Init time: %.0fmsec\n", t );
	}

	// post-init stuff
	// can no longer modify cvars flagged with CVAR_INIT
	Cvar::initialised = true;

	// frame
	Common::gameTimer = new Timer(); // TODO: free

	real64_t t = 0.0;
	real64_t currentTime = Common::gameTimer->GetTiming( false, TimerResolution::Milliseconds );
	real64_t accumulator = 0.0;

	while ( 1 ) {
		// calculate delta time for integrating this frame
		const real64_t frameStartTime = Common::gameTimer->GetTiming( false, TimerResolution::Milliseconds );
		const real64_t frameTime = frameStartTime - currentTime;
		currentTime = frameStartTime;

		// avoid spiral of death
		real64_t sliceMsec = frameTime;
		if ( sliceMsec > 250.0 ) {
			sliceMsec = 250.0;
		}
		accumulator += sliceMsec;

		// input
		if ( !dedicated ) {
			//TODO: run on another thread at 1000hz?
			Client::Input::Poll();
		}

		// event pump
		Event::Pump();
		Command::ExecuteBuffer();

		Timer perf;

		const real64_t dt = dedicated
			? 1000.0 / Common::com_framerate->GetReal64()
			: 1000.0 / Common::r_framerate->GetReal64();

		//FIXME: is it better to run the server frame, and then the client frame? or the other way around
		//	we don't want the client acting on old server data, but we also don't want the client's input to be delayed
		//		by a frame

		if ( dedicated ) {
			// server tick
			while ( accumulator >= dt ) {
				Server::RunFrame( dt );
				accumulator -= dt;
				t += dt;
			}
			if ( Common::com_profile->GetBool() ) {
				console.Print( PrintLevel::Developer, "time: server frame took %.3fmsec\n",
					perf.GetTiming( true, TimerResolution::Milliseconds )
				);
			}

			// broadcast state changes
			Server::NetworkPump();
			if ( Common::com_profile->GetBool() ) {
				console.Print( PrintLevel::Developer, "time: server network pump took %.3fmsec\n",
					perf.GetTiming( true, TimerResolution::Milliseconds )
				);
			}
		}
		else {
			// client tick
			// recieve server update
			// create movement command and send it off
			Client::NetworkPump();
			if ( Common::com_profile->GetBool() ) {
				console.Print( PrintLevel::Developer, "time: client network pump took %.3fmsec\n",
					perf.GetTiming( true, TimerResolution::Milliseconds )
				);
			}

			// run local prediction from latest server snapshot
			Client::RunFrame( sliceMsec );
			if ( Common::com_profile->GetBool() ) {
				console.Print( PrintLevel::Developer, "time: client frame took %.3fmsec\n",
					perf.GetTiming( true, TimerResolution::Milliseconds )
				);
			}

			// generate the renderer workload
			// alpha = accumulator / dt;
			// lerp( previousState, alpha, currentState )
			Client::DrawFrame( sliceMsec );
			if ( Common::com_profile->GetBool() ) {
				console.Print( PrintLevel::Developer, "time: client render took %.3fmsec\n",
					perf.GetTiming( true, TimerResolution::Milliseconds )
				);
			}

			// consume buffered render commands
			Renderer::Update( sliceMsec/*state*/ );
			if ( Common::com_profile->GetBool() ) {
				console.Print( PrintLevel::Developer, "time: render update took %.3fmsec\n",
					perf.GetTiming( true, TimerResolution::Milliseconds )
				);
			}
		}

		const real64_t targetMsec = dt; // target mspf
		const real64_t realFT = Common::gameTimer->GetTiming( false, TimerResolution::Milliseconds )
			- frameStartTime;
		const real64_t delayMsec = targetMsec - realFT;
		if ( Common::com_profile->GetBool() ) {
			console.Print( PrintLevel::Debug, "frame took %.5f, target %.2f, delay for: %i ? 0.000 : %.3f\n",
				realFT,
				targetMsec,
				!!(realFT < targetMsec),
				delayMsec
			);
		}
		if ( realFT < targetMsec ) {
			if ( Common::com_busyWait->GetBool() ) {
				real64_t startTime = Common::gameTimer->GetTiming( false, TimerResolution::Milliseconds );
				real64_t delayAccum = 0.0;
				while ( delayAccum < delayMsec ) {
					Indent indent( 1u );

					// if targetMsec is 8msec and realFT is 6msec
					// then wait for 2msec (targetMsec(8) - realFT(6) = 2)
					const real64_t nowTime = Common::gameTimer->GetTiming(
						false,
						TimerResolution::Milliseconds
					);
					delayAccum = nowTime - startTime;

					// force the OS to reschedule tasks
					SDL_Delay( 0u );
				}
			}
			else {
				// scheduler granularity is ~10ms, so it's not ideal to use for regulating framerate, but users may
				//	wish to save power (i.e. portable or low-power devices)
				SDL_Delay( static_cast<uint32_t>( delayMsec ) );
			}
		}
	}
}

catch( const XSError &e ) {

	const bool profile = Common::com_profile->GetBool();
	const bool dedicated = Common::com_dedicated->GetBool();

	if ( !e.intended ) {
		SDL_assert( !"A fatal error has occurred. Please check your console.log.\nPress \"ignore\" to proceed with "
			"shutdown" );
	}

	console.Print( PrintLevel::Normal, "\n*** " PRODUCT_NAME " is shutting down\n" );
	if ( !e.intended && e.what() ) {
		console.Print( PrintLevel::Normal, "Reason: %s\n", e.what() );
	}
	console.Print( PrintLevel::Normal, "\n" );

	if ( profile ) {
		const real64_t runtime = globalTimer.GetTiming( true, TimerResolution::Seconds );
		console.Print( PrintLevel::Normal, "Run time: %.3fsec\n", runtime );
	}

	delete Common::gameTimer;

	// indent the console for this scope
	{
		Indent indent( 1 );
		if ( dedicated ) {
			Server::Shutdown();
		}
		else {
			Client::Shutdown();
		}
		Network::Shutdown();

		if ( dedicated ) {
			Renderer::Shutdown();
		}

		if ( Cvar::initialised ) {
			// only write out the configuration if xsngine was able to fully initialise, else we'll be writing
			//	default values
			Common::WriteConfig();
			Cvar::Clean();
		}
	}

	if ( profile ) {
		const real64_t shutdownTime = globalTimer.GetTiming( false, TimerResolution::Milliseconds );
		console.Print( PrintLevel::Normal, "Shutdown time: %.3fmsec\n\n\n", shutdownTime );
	}

	return EXIT_SUCCESS;
}
