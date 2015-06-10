#include <algorithm>

#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSMatrix.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSEvent.h"
#include "XSClient/XSClientGame.h"
#include "XSClient/XSClientGameState.h"
#include "XSClient/XSCheckersBoard.h"
#include "XSInput/XSInput.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSModel.h"

namespace XS {

	namespace ClientGame {

		Cvar *cg_fov = nullptr;

		static Renderer::View *checkersView = nullptr;
		static CheckersBoard *board = nullptr;

		static void RegisterCvars( void ) {
			//fov = degrees( pi * 0.75 ) / 1.25 because math
			cg_fov = Cvar::Create( "cg_fov", "108", "Field of view", CVAR_ARCHIVE );
		}

		static void RegisterCommands( void ) {
			// ...
		}

		static void RenderScene( real64_t dt ) {
			// no camera stuff to do
		}

		void Init( void ) {
			RegisterCvars();
			RegisterCommands();

			checkersView = new Renderer::View( 0u, 0u, true, RenderScene );
			board = new CheckersBoard();
		}

		void Shutdown( void ) {
			delete checkersView;
			delete board;
		}

		void RunFrame( real64_t dt ) {
			board->Update( dt );
		}

		void DrawFrame( real64_t dt ) {
			checkersView->Bind();
			board->Render( dt );
		}

		void MouseMotionEvent( const struct MouseMotionEvent &ev ) {
			vector3 delta( ev.y, ev.x, 0.0f );

			console.Print( PrintLevel::Normal, "%s: %i/%i\n", XS_FUNCTION, ev.x, ev.y );

			state.cursorPos += vector2( ev.x, ev.y );
		}

		void MouseButtonEvent( const struct MouseButtonEvent &ev ) {
			if ( ev.pressed && ev.button == SDL_BUTTON_LEFT ) {
				console.Print( PrintLevel::Normal,
					"%s: %.2f/%.2f\n",
					XS_FUNCTION,
					state.cursorPos.x,
					state.cursorPos.y
				);
			}
		}

	} // namespace ClientGame

} // namespace XS
