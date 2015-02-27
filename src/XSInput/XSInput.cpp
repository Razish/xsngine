#include <deque>

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

		namespace Input {

			Cvar *m_acceleration = nullptr;
			Cvar *m_debug = nullptr;
			Cvar *m_sensitivity = nullptr;
			Cvar *m_smooth = nullptr;
			Cvar *m_smoothFrames = nullptr;

			static void RegisterCvars( void ) {
				m_acceleration = Cvar::Create( "m_acceleration", "0", "Enable mouse acceleration", CVAR_ARCHIVE );
				m_debug = Cvar::Create( "m_debug", "0", "Show debugging information for mouse input", CVAR_ARCHIVE );
				m_sensitivity = Cvar::Create( "m_sensitivity", "1", "Sensitivity of mouse input", CVAR_ARCHIVE );
				m_smooth = Cvar::Create( "m_smoothMouse", "0", "Smooth the mouse input across multiple frames",
					CVAR_ARCHIVE );
				m_smoothFrames = Cvar::Create( "m_smoothFrames", "2", "How many frames to smooth the mouse input "
					"across", CVAR_ARCHIVE );
			}

			void Init( void ) {
				RegisterCvars();

				// raw input
				// this is also set in ClientConsole upon (de)activation
				SDL_SetRelativeMouseMode( SDL_TRUE );
			}

			void Poll( void ) {
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
							console.Print( PrintLevel::Developer, "%s: Window %d shown\n",
								funcName,
								e.window.windowID
							);
						} break;
						case SDL_WINDOWEVENT_HIDDEN: {
							console.Print( PrintLevel::Developer, "%s: Window %d hidden\n",
								funcName,
								e.window.windowID
							);
						} break;
						case SDL_WINDOWEVENT_EXPOSED: {
							console.Print( PrintLevel::Developer, "%s: Window %d exposed\n",
								funcName,
								e.window.windowID
							);
						} break;
						case SDL_WINDOWEVENT_MOVED: {
							console.Print( PrintLevel::Developer, "%s: Window %d moved to %d,%d\n",
								funcName,
								e.window.windowID,
								e.window.data1,
								e.window.data2
							);
						} break;
						case SDL_WINDOWEVENT_RESIZED: {
							console.Print( PrintLevel::Developer, "%s: Window %d resized to %dx%d\n",
								funcName,
								e.window.windowID,
								e.window.data1,
								e.window.data2
							);
						} break;
						case SDL_WINDOWEVENT_MINIMIZED: {
							console.Print( PrintLevel::Developer, "%s: Window %d minimized\n",
								funcName,
								e.window.windowID
							);
						} break;
						case SDL_WINDOWEVENT_MAXIMIZED: {
							console.Print( PrintLevel::Developer, "%s: Window %d maximized\n",
								funcName,
								e.window.windowID
							);
						} break;
						case SDL_WINDOWEVENT_RESTORED: {
							console.Print( PrintLevel::Developer, "%s: Window %d restored\n",
								funcName,
								e.window.windowID
							);
						} break;
						case SDL_WINDOWEVENT_ENTER: {
							console.Print( PrintLevel::Developer, "%s: Mouse entered window %d\n",
								funcName,
								e.window.windowID
							);
						} break;
						case SDL_WINDOWEVENT_LEAVE: {
							console.Print( PrintLevel::Developer, "%s: Mouse left window %d\n",
								funcName,
								e.window.windowID
							);
						} break;
						case SDL_WINDOWEVENT_FOCUS_GAINED: {
							console.Print( PrintLevel::Developer, "%s: Window %d gained keyboard focus\n",
								funcName,
								e.window.windowID
							);
						} break;
						case SDL_WINDOWEVENT_FOCUS_LOST: {
							console.Print( PrintLevel::Developer, "%s: Window %d lost keyboard focus\n",
								funcName,
								e.window.windowID
							);
						} break;
						case SDL_WINDOWEVENT_CLOSE: {
							console.Print( PrintLevel::Developer, "%s: Window %d closed\n",
								funcName,
								e.window.windowID
							);
						} break;
						default: {
							console.Print( PrintLevel::Developer, "%s: Window %d got unknown event %d\n",
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
						const int32_t relativeMotion[2] = { e.motion.xrel, e.motion.yrel };
						//const int32_t absolutePosition[2] = { e.motion.x, e.motion.y };
						/*
						console.Print( PrintLevel::Normal,
							"Mouse motion: d( %i,%i ) - a( %i,%i )\n",
							relativeMotion[0], relativeMotion[1],
							absolutePosition[0], absolutePosition[1]
						);
						*/
						XSEvent ev( EventType::MouseMotion );

						ev.mouseMotion.x = relativeMotion[0];
						ev.mouseMotion.y = relativeMotion[1];

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

						//FIXME: is this really where the keystate should be set? or should it be set when the list
						//	 is pumped
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

			static std::deque<MovementCommand> movementCmds;
			static MovementCommand &AllocateMovementCommand( void ) {
				//FIXME: calculate ~1s worth of input?
				if ( movementCmds.size() >= 10 ) {
					movementCmds.pop_front();
					//FIXME: does this call destructor on pointer elements?
				}

				MovementCommand newCmd = {};
				movementCmds.push_back( newCmd );

				return movementCmds.back();
			}

			MovementCommand GenerateMovementCommand( real64_t dt ) {
				//TODO: keep a list of previously generated movement commands to resend
				MovementCommand &cmd = AllocateMovementCommand();

				//TODO: get button state etc

				CalculateMouseMotion( dt, cmd );

				return cmd;
			}

		} // namespace Input

	} // namespace Client

} // namespace XS
