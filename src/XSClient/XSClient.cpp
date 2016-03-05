#include <RakNet/RakPeerInterface.h>

#include <stack>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSVector.h"
#include "XSCommon/XSTimer.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSClientGame.h"
#include "XSClient/XSClientConsole.h"
#include "XSClient/XSMenuManager.h"
#include "XSClient/XSTerrain.h"
#include "XSInput/XSInput.h"
#include "XSInput/XSMouse.h"
#include "XSInput/XSKeys.h"
#include "XSNetwork/XSNetwork.h"
#include "XSRenderer/XSFont.h"
#include "XSRenderer/XSView.h"

namespace XS {

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

		void Cmd_ToggleConsole( const CommandContext & context ) {
			if ( !clientConsole ) {
				throw XSError( "Tried to toggle client console without a valid instance" );
			}

			clientConsole->Toggle();
		}

		static void Cmd_Disconnect( const CommandContext & context ) {
			if ( !Network::IsConnected() ) {
				console.Print( PrintLevel::Normal, "Not connected to a server\n" );
				return;
			}
			Network::Disconnect();
		}

		static void Cmd_SendCommand( const CommandContext & context ) {
			if ( !Network::IsConnected() ) {
				console.Print( PrintLevel::Normal, "Not connected to a server\n" );
				return;
			}

			ByteBuffer commandBuffer;

			// write the number of args
			const size_t numArgs = context.size() - 1;
			commandBuffer.WriteUInt32( numArgs );

			// write each arg
			for ( const std::string &arg : context ) {
				commandBuffer.WriteString( arg.c_str() );
				console.Print( PrintLevel::Normal, "  arg: %s, len: %i\n",
					arg.c_str(), arg.length()
				);
			}

			Network::XSPacket commandPacket( Network::ID_XS_CL2SV_COMMAND );
			commandPacket.data = commandBuffer.GetMemory( &commandPacket.dataLen );
			commandPacket.Send( 0u );
		}

		static void Cmd_Connect( const CommandContext & context ) {
			size_t numArgs = context.size();
			const char *hostname = (numArgs >= 1) ? context[0].c_str() : nullptr;
			uint16_t port = (numArgs >= 2) ? atoi( context[1].c_str() ) : 0u;
			Network::Connect( hostname, port );
		}

		static void Cmd_OpenMenu( const CommandContext & context ) {
			menu->OpenMenu( context[0].c_str() );
		}

		static void Cmd_ReloadMenu( const CommandContext & context ) {
			delete menu;
			menu = new MenuManager();
			menu->RegisterMenu( "menus/settings.xmenu" );
		}

		static void RegisterCommands( void ) {
			Command::AddCommand( "disconnect", Cmd_Disconnect );
			Command::AddCommand( "cmd", Cmd_SendCommand );
			Command::AddCommand( "connect", Cmd_Connect );
			Command::AddCommand( "openMenu", Cmd_OpenMenu );
			Command::AddCommand( "reloadMenu", Cmd_ReloadMenu );
		}

		bool MouseMotionEvent( const struct MouseMotionEvent &ev ) {
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

			if ( menu->isOpen ) {
				menu->MouseMotionEvent( ev );
				return true;
			}

			else if ( !clientConsole || !clientConsole->IsVisible() ) {
				ClientGame::MouseMotionEvent( ev );
			}

			return false;
		}

		bool MouseButtonEvent( const struct MouseButtonEvent &ev ) {
			if ( menu->isOpen ) {
				menu->MouseButtonEvent( ev );
				return true;
			}

			ClientGame::MouseButtonEvent( ev );

			return false;
		}

		void KeyboardEvent( const struct KeyboardEvent &ev ) {
			// hardcoded console short-circuit
			if ( clientConsole ) {
				if ( ev.down && ev.key == SDLK_BACKQUOTE ) {
					clientConsole->Toggle();
					return;
				}
				else if ( clientConsole->KeyboardEvent( ev ) ) {
					return;
				}
			}

			// let the menu consume key events
			if ( menu->isOpen ) {
				menu->KeyboardEvent( ev );
				return;
			}

			// fall through to gamecode
			keystate[ev.key] = ev.down;
			ExecuteBind( ev );
		}

		void Init( void ) {
			RegisterCommands();

			Network::Init();

			// client game module
			ClientGame::Init();

			// hud
			hudView = new Renderer::View( 0u, 0u, true );
			menu = new MenuManager();
			menu->RegisterMenu( "menus/settings.xmenu" );
		//	menu->OpenMenu( "settings" );

			// console
			clientConsole = new ClientConsole( &console );
		}

		void Shutdown( void ) {
			delete clientConsole;
			delete menu;
			delete hudView;

			ClientGame::Shutdown();

			Network::Disconnect();
			Network::Shutdown();
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
				Renderer::rdState.window.width - textWidth,
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

} // namespace XS
