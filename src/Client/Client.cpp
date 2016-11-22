#include <RakNet/RakPeerInterface.h>

#include <stack>

#include "Common/Common.h"
#include "Common/Error.h"
#include "Common/String.h"
#include "Common/Cvar.h"
#include "Common/Console.h"
#include "Common/Vector.h"
#include "Common/Timer.h"
#include "Common/Globals.h"
#include "Common/Event.h"
#include "Common/ByteBuffer.h"
#include "Client/Client.h"
#include "Client/ClientGame.h"
#include "Client/ClientGameState.h"
#include "Client/ClientConsole.h"
#include "Client/MenuManager.h"
#include "Client/Terrain.h"
#include "Input/Input.h"
#include "Input/Mouse.h"
#include "Input/Keys.h"
#include "Network/Network.h"
#include "Renderer/Font.h"
#include "Renderer/View.h"

namespace Client {

	uint64_t frameNum = 0u;

	// hud
	static Renderer::View *hudView = nullptr;

	// menus
	MenuManager *menu = nullptr;

	//FIXME: move this to local client storage, not ClientGame
	//	kind of related to menus, but we also want it available for 2d games. it belongs in neither, so it should
	//	live in the Client namespace
	vector2 cursorPos = { 0.5f, 0.5f };	// used for menus and 2d games

	// console input
	ClientConsole *clientConsole = nullptr;

	void Cmd_ToggleConsole( const CommandContext &context ) {
		if ( !clientConsole ) {
			throw XSError( "Tried to toggle client console without a valid instance" );
		}

		clientConsole->Toggle();
	}

	static void Cmd_Disconnect( const CommandContext &context ) {
		if ( !Network::IsConnected() ) {
			console.Print( PrintLevel::Normal, "Not connected to a server\n" );
			return;
		}
		Network::Disconnect();
	}

	static void Cmd_SendCommand( const CommandContext &context ) {
		if ( !Network::IsConnected() ) {
			console.Print( PrintLevel::Normal, "Not connected to a server\n" );
			return;
		}

		ByteBuffer commandBuffer;
		ByteBuffer::Error status;

		// write the number of args
		const size_t numArgs = context.size() - 1;
		status = commandBuffer.Write<uint32_t>( numArgs );

		// write each arg
		for ( const std::string &arg : context ) {
			status = commandBuffer.WriteString( arg.c_str() );
		}

		if ( status == ByteBuffer::Error::Success ) {
			Network::XSPacket commandPacket( Network::ID_XS_CL2SV_COMMAND );
			commandPacket.data = commandBuffer.GetMemory( &commandPacket.dataLen );
			commandPacket.Send( 0u );
		}
	}

	static void Cmd_Connect( const CommandContext &context ) {
		size_t numArgs = context.size();
		const char *hostname = (numArgs >= 1) ? context[0].c_str() : nullptr;
		uint16_t port = (numArgs >= 2) ? atoi( context[1].c_str() ) : 0u;
		Network::Connect( hostname, port );
	}

	static void Cmd_OpenMenu( const CommandContext &context ) {
		menu->OpenMenu( context[0].c_str() );
	}

	static void Cmd_ReloadMenu( const CommandContext &context ) {
		delete menu;
		menu = new MenuManager( *hudView );
		menu->RegisterMenu( "menus/main.xmenu" );
	}

	static void RegisterCommands( void ) {
		Command::AddCommand( "disconnect", Cmd_Disconnect );
		Command::AddCommand( "cmd", Cmd_SendCommand );
		Command::AddCommand( "connect", Cmd_Connect );
		Command::AddCommand( "openMenu", Cmd_OpenMenu );
		Command::AddCommand( "reloadMenu", Cmd_ReloadMenu );
	}

	bool KeyboardEvent( const struct KeyboardEvent &ev ) {
		if ( clientConsole ) {
			// hardcoded console short-circuit
			if ( ev.down && ev.key == SDLK_BACKQUOTE ) {
				clientConsole->Toggle();
				return true;
			}
			else if ( clientConsole->KeyboardEvent( ev ) ) {
				return true;
			}
		}

		// let the menu consume key events
		if ( menu->isOpen ) {
			if ( menu->KeyboardEvent( ev ) ) {
				return true;
			}
		}

		// fall through to gamecode
		//FIXME: move to ClientGame::KeyboardEvent
		keystate[ev.key] = ev.down;
		ExecuteBind( ev );

		return true;
	}

	bool MouseButtonEvent( const struct MouseButtonEvent &ev ) {
		if ( clientConsole ) {
			if ( clientConsole->MouseButtonEvent( ev ) ) {
				return true;
			}
		}

		if ( menu->isOpen ) {
			if ( menu->MouseButtonEvent( ev ) ) {
				return true;
			}
		}

		if ( ClientGame::MouseButtonEvent( ev ) ) {
			return true;
		}

		return false;
	}

	bool MouseMotionEvent( const struct MouseMotionEvent &ev ) {
		if ( clientConsole ) {
			if ( clientConsole->MouseMotionEvent( ev ) ) {
				return true;
			}
		}

		if ( menu->isOpen ) {
			cursorPos[0] += ev.x / static_cast<real32_t>( Renderer::rdState.window.width );
			cursorPos[1] += ev.y / static_cast<real32_t>( Renderer::rdState.window.height );

			// clamp the cursor coordinates to screen-space
			if ( cursorPos[0] < 0.0f ) {
				cursorPos[0] = 0.0f;
			}
			else if ( cursorPos[0] > 1.0f ) {
				cursorPos[0] = 1.0f;
			}
			if ( cursorPos[1] < 0.0f ) {
				cursorPos[1] = 0.0f;
			}
			else if ( cursorPos[1] > 1.0f ) {
				cursorPos[1] = 1.0f;
			}
			if ( menu->MouseMotionEvent( ev ) ) {
				return true;
			}
		}

		if ( ClientGame::MouseMotionEvent( ev ) ) {
			return true;
		}

		return false;
	}

	bool MouseWheelEvent( const struct MouseWheelEvent &ev ) {
		if ( clientConsole ) {
			if ( clientConsole->MouseWheelEvent( ev ) ) {
				return true;
			}
		}

		if ( menu->isOpen ) {
			if ( menu->MouseWheelEvent( ev ) ) {
				return true;
			}
		}

		if ( ClientGame::MouseWheelEvent( ev ) ) {
			return true;
		}

		return false;
	}

	void Init( void ) {
		RegisterCommands();

		// client game module
		ClientGame::Init();

		// hud
		hudView = new Renderer::View( 0u, 0u, true );
		menu = new MenuManager( *hudView );
		menu->RegisterMenu( "menus/main.xmenu" );
		menu->RegisterMenu( "menus/settings.xmenu" );
		menu->OpenMenu( "main" );

		// console
		clientConsole = new ClientConsole( &console );
	}

	void Shutdown( void ) {
		delete clientConsole;
		delete menu;
		delete hudView;

		ClientGame::Shutdown();

		Network::Disconnect();
	}

	void NetworkPump( void ) {
		if ( !Network::IsActive() ) {
			return;
		}

		// handle generic messages
		Network::Receive();

		// generate this frame's movement command
		Input::MovementCommand &cmd = Input::AllocateMovementCommand();
		Input::GenerateMovementCommand( cmd );
	}

	bool ReceivePacket( const RakNet::Packet *packet ) {
		if ( Network::IsConnected() ) {
			return ClientGame::ReceivePacket( packet );
		}
		else {
			return false;
		}
	}

	void Connect( Network::Connection &connection ) {
		// the server has accepted our connection
		if ( ClientGame::clgState.connection ) {
			//TODO: print message about disconnecting from current server
		}

		ClientGame::clgState.connection = &connection;

		// request a connection
		connection.ChangeState( Network::Connection::State::SynSent, true );
	}

	void Disconnect( Network::GUID guid ) {
		// the server has disconnected us
		if ( !ClientGame::clgState.connection || ClientGame::clgState.connection->guid != guid ) {
			return;
		}
		ClientGame::clgState.connection = nullptr;
	}

	void RunFrame( real64_t dt ) {
		static real64_t stepTime = 0.0;
		frameNum++;

		// previousState = currentState;
		// integrate( currentState, stepTime, dt );
		stepTime += dt;

		// process server updates
		// simulate local entities
		//	predict entities whose state is not managed by the server, created by either the client or server
		//	e.g. client may create its own projectiles until the server overrides it
		// movement prediction (movement commands have been generated by input poll)
		ClientGame::RunFrame( dt );
	}

	// lazy initialise on first request per frame
	real64_t GetElapsedTime( TimerResolution resolution ) {
		static uint64_t lastFrame = 0u;
		static real64_t timeSec = 0.0;
		static real64_t timeMsec = 0.0;
		static real64_t timeUsec = 0.0;
		if ( lastFrame != frameNum ) {
			lastFrame = frameNum;
			timeUsec = Common::gameTimer->GetTiming();
			timeMsec = timeUsec * 0.001;
			timeSec = timeUsec * 0.000001;
		}

		switch ( resolution ) {

			case TimerResolution::Seconds: {
				return timeSec;
			} break;

			case TimerResolution::Milliseconds: {
				return timeMsec;
			} break;

			case TimerResolution::Microseconds: {
				return timeUsec;
			} break;

			default: {
				return 0.0;
			} break;
		}
	}

	static void DrawFPS( real64_t frametime, Renderer::Font *font ) {
		static const uint32_t numSamples = 64u;
		static real64_t samples[numSamples];
		static uint32_t index = 0;
		samples[index++] = frametime;
		if ( index >= numSamples ) {
			index = 0u;
		}
		real64_t avg = 0.0;
		for ( uint32_t i = 0; i < numSamples; i++ ) {
			avg += samples[i];
		}
		avg /= static_cast<real64_t>( numSamples );

		std::string fpsText = String::Format( "FPS:%.0f", 1000.0 / avg ).c_str();
		const uint16_t fpsTextSize = 16u;
		real32_t textWidth = 2.0f;
		for ( const char *p = fpsText.c_str(); *p; p++ ) {
			textWidth += font->GetGlyphWidth( *p, fpsTextSize );
		}

		vector2 pos = {
			hudView->width - textWidth,
			0.0f
		};
		font->Draw( pos, fpsText, fpsTextSize );
	}

	static void DrawMenus( Renderer::Font *font ) {
		if ( menu->isOpen ) {
			menu->PaintMenus();
			menu->DrawCursor();
		}
	}

	static void DrawDebugInfo( Renderer::Font *font ) {
		// ...
	}

	static void DrawHUD( real64_t frametime ) {
		if ( !hudView ) {
			return;
		}

		hudView->Bind();

		static Renderer::Font *font = nullptr;
		if ( !font ) {
			font = Renderer::Font::Register( "menu" );
		}

		DrawFPS( frametime, font );

		DrawMenus( font );

		DrawDebugInfo( font );
	}

	void DrawFrame( real64_t frametime ) {
		// draw game view
		ClientGame::DrawFrame( frametime );

		// draw HUD
		DrawHUD( frametime );

		// draw console
		clientConsole->Draw();
	}

} // namespace Client
