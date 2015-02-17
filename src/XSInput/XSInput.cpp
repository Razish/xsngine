#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSConsole.h"
#include "XSInput/XSInput.h"
#include "XSInput/XSKeys.h"

namespace XS {

	namespace Client {

		// Client input instance. Access via Client::input.Blah()
		Input input;

		Input::Input() {
			// raw input
			// this is also set in ClientConsole upon (de)activation
			SDL_SetRelativeMouseMode( SDL_TRUE );
		}

		void Input::Poll( void ) const {
			SDL_Event e;
			while ( SDL_PollEvent( &e ) ) {
				switch ( e.type ) {
				case SDL_QUIT: {
					throw( XSError( "Quit application" ) );
				} break;

				case SDL_WINDOWEVENT: {
					static const char *funcName = XS_FUNCTION_VERBOSE;
					switch ( e.window.event ) {
					case SDL_WINDOWEVENT_SHOWN: {
						console.Print( PrintLevel::Debug, "%s: Window %d shown\n",
							funcName,
							e.window.windowID
						);
					} break;
					case SDL_WINDOWEVENT_HIDDEN: {
						console.Print( PrintLevel::Debug, "%s: Window %d hidden\n",
							funcName,
							e.window.windowID
						);
					} break;
					case SDL_WINDOWEVENT_EXPOSED: {
						console.Print( PrintLevel::Debug, "%s: Window %d exposed\n",
							funcName,
							e.window.windowID
						);
					} break;
					case SDL_WINDOWEVENT_MOVED: {
						console.Print( PrintLevel::Debug, "%s: Window %d moved to %d,%d\n",
							funcName,
							e.window.windowID,
							e.window.data1,
							e.window.data2
						);
					} break;
					case SDL_WINDOWEVENT_RESIZED: {
						console.Print( PrintLevel::Debug, "%s: Window %d resized to %dx%d\n",
							funcName,
							e.window.windowID,
							e.window.data1,
							e.window.data2
						);
					} break;
					case SDL_WINDOWEVENT_MINIMIZED: {
						console.Print( PrintLevel::Debug, "%s: Window %d minimized\n",
							funcName,
							e.window.windowID
						);
					} break;
					case SDL_WINDOWEVENT_MAXIMIZED: {
						console.Print( PrintLevel::Debug, "%s: Window %d maximized\n",
							funcName,
							e.window.windowID
						);
					} break;
					case SDL_WINDOWEVENT_RESTORED: {
						console.Print( PrintLevel::Debug, "%s: Window %d restored\n",
							funcName,
							e.window.windowID
						);
					} break;
					case SDL_WINDOWEVENT_ENTER: {
						console.Print( PrintLevel::Debug, "%s: Mouse entered window %d\n",
							funcName,
							e.window.windowID
						);
					} break;
					case SDL_WINDOWEVENT_LEAVE: {
						console.Print( PrintLevel::Debug, "%s: Mouse left window %d\n",
							funcName,
							e.window.windowID
						);
					} break;
					case SDL_WINDOWEVENT_FOCUS_GAINED: {
						console.Print( PrintLevel::Debug, "%s: Window %d gained keyboard focus\n",
							funcName,
							e.window.windowID
						);
					} break;
					case SDL_WINDOWEVENT_FOCUS_LOST: {
						console.Print( PrintLevel::Debug, "%s: Window %d lost keyboard focus\n",
							funcName,
							e.window.windowID
						);
					} break;
					case SDL_WINDOWEVENT_CLOSE: {
						console.Print( PrintLevel::Debug, "%s: Window %d closed\n",
							funcName,
							e.window.windowID
						);
					} break;
					default: {
						console.Print( PrintLevel::Debug, "%s: Window %d got unknown event %d\n",
							funcName,
							e.window.windowID,
							e.window.event
						);
					} break;
					}
				} break;

				case SDL_TEXTINPUT: {
					// ...
				} break;

				case SDL_MOUSEMOTION: {
					//const int32_t relativeMotion[2] = { e.motion.xrel, e.motion.yrel };
					//const int32_t absolutePosition[2] = { e.motion.x, e.motion.y };
				} break;

				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP: {
					XSEvent ev( EventType::MOUSEBUTTON );
					ev.mouseButton.button = e.button.button;
					ev.mouseButton.pressed = (e.button.state == SDL_PRESSED) ? true : false;
					//ev.mouseButton.position[0] = e.button.x;
					//ev.mouseButton.position[1] = e.button.y;

					Event::Queue( &ev );
				} break;

				case SDL_MOUSEWHEEL: {
					XSEvent ev( EventType::MOUSEWHEEL );
					ev.mouseWheel.up = (e.wheel.y > 0);
					ev.mouseWheel.amount = std::abs( e.wheel.y );

					Event::Queue( &ev );
				} break;

				case SDL_KEYDOWN:
				case SDL_KEYUP: {
					SDL_Keycode key = e.key.keysym.sym;
					bool pressed = (e.key.state == SDL_PRESSED);

					keystate[key] = pressed;

					XSEvent ev( EventType::KEY );
					ev.keyEvent.key = key;
					ev.keyEvent.down = pressed;

					Event::Queue( &ev );
				} break;

				default: {
					console.Print( PrintLevel::Normal, "Unhandled SDL event %d\n",
						e.type
					);
				} break;
				}
			}

			//TODO: movement command generation?
		}

	} // namespace Client

} // namespace XS
