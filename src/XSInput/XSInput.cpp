#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSConsole.h"
#include "XSInput/XSInput.h"
#include "XSInput/XSKeys.h"
#include "XSInput/XSMouse.h"

namespace XS {

	namespace Client {

		// Client input instance. Access via Client::input.Blah()
		Input input;
		Cvar *m_acceleration = nullptr;
		Cvar *m_sensitivity = nullptr;
		Cvar *m_smooth = nullptr;
		Cvar *m_smoothFrames = nullptr;

		Input::Input() {
			m_acceleration = Cvar::Create( "m_acceleration", "0", "Enable mouse acceleration", CVAR_ARCHIVE );
			m_sensitivity = Cvar::Create( "m_sensitivity", "1", "Sensitivity of mouse input", CVAR_ARCHIVE );
			m_smooth = Cvar::Create( "m_smoothMouse", "0", "Smooth the mouse input across multiple frames",
				CVAR_ARCHIVE );
			m_smoothFrames = Cvar::Create( "m_smoothFrames", "1", "How many frames to smooth the mouse input across",
				CVAR_ARCHIVE );

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

				// ignoring these for now...
				case SDL_FINGERUP:
				case SDL_FINGERDOWN:
				case SDL_TEXTINPUT: {
					// ...
				} break;

				case SDL_FINGERMOTION: {
					/*
					const real32_t relativeMotion[2] = { e.tfinger.dx, e.tfinger.dy };
					const real32_t absolutePosition[2] = { e.tfinger.x, e.tfinger.y };
					console.Print( PrintLevel::Normal,
						"Touch motion: d( %.3f,%.3f ) - a( %.3f,%.3f )\n",
						relativeMotion[0], relativeMotion[1],
						absolutePosition[0], absolutePosition[1]
					);
					*/
				} break;
				case SDL_MOUSEMOTION: {
				//	const int32_t relativeMotion[2] = { e.motion.xrel, e.motion.yrel };
				//	const int32_t absolutePosition[2] = { e.motion.x, e.motion.y };
					/*
					console.Print( PrintLevel::Normal,
						"Mouse motion: d( %i,%i ) - a( %i,%i )\n",
						relativeMotion[0], relativeMotion[1],
						absolutePosition[0], absolutePosition[1]
					);
					*/
					XSEvent ev( EventType::MouseMotion );
					Event::Queue( &ev );
				} break;

				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP: {
					XSEvent ev( EventType::MouseButton );
					ev.mouseButton.button = e.button.button;
					ev.mouseButton.pressed = (e.button.state == SDL_PRESSED) ? true : false;
					//ev.mouseButton.position[0] = e.button.x;
					//ev.mouseButton.position[1] = e.button.y;

					Event::Queue( &ev );
				} break;

				case SDL_MOUSEWHEEL: {
					XSEvent ev( EventType::MouseWheel );
					ev.mouseWheel.up = (e.wheel.y > 0);
					ev.mouseWheel.amount = std::abs( e.wheel.y );

					Event::Queue( &ev );
				} break;

				case SDL_MULTIGESTURE: {
					XSEvent ev( EventType::MouseWheel );

					//FIXME: translate gesture events into actual mouse wheel events
					ev.mouseWheel.up = true;
					ev.mouseWheel.amount = 1u;

					Event::Queue( &ev );
				} break;

				case SDL_KEYDOWN:
				case SDL_KEYUP: {
					SDL_Keycode key = e.key.keysym.sym;
					bool pressed = (e.key.state == SDL_PRESSED);

					keystate[key] = pressed;

					XSEvent ev( EventType::Keyboard );
					ev.keyboard.key = key;
					ev.keyboard.down = pressed;
					Event::Queue( &ev );
				} break;

				default: {
					console.Print( PrintLevel::Normal, "Unhandled SDL event %d\n",
						e.type
					);
				} break;
				}
			}
		}

		MovementCommand GenerateMovementCommand( real64_t dt ) {
			MovementCommand dummy;

			//TODO: get button state etc

			CalculateMouseMotion( dt, dummy );

			return dummy;
		}

	} // namespace Client

} // namespace XS
