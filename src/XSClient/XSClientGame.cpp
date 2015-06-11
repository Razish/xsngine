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

		static Renderer::View *checkersView = nullptr;
		static CheckersBoard *board = nullptr;

		static void RenderScene( real64_t dt ) {
			// no camera stuff to do
		}

		void Init( void ) {
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
			state.cursorPos += vector2( ev.x, ev.y );
		}

		void MouseButtonEvent( const struct MouseButtonEvent &ev ) {
			if ( ev.pressed && ev.button == SDL_BUTTON_LEFT ) {
				board->MouseButtonEvent( state.cursorPos.x, state.cursorPos.y );
			}
		}

	} // namespace ClientGame

} // namespace XS
