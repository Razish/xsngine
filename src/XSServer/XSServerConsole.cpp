#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#elif defined(XS_OS_WINDOWS)
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#ifndef NOMINMAX
	#define NOMINMAX /* Don't define min() and max() */
#endif
#include <Windows.h>
#endif

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSServer/XSServerConsole.h"
#include "XSServer/XSServer.h"

namespace XS {

	namespace Server {

#if defined(XS_OS_WINDOWS)
		static const SHORT bufferSize = 80;
#else
		static const size_t bufferSize = 80u;
#endif

#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
		const char *ServerConsole::promptText = "tty] ";
		int ServerConsole::ec_erase = 0;
		int ServerConsole::ec_eof = 0;
#endif

#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
		static void SigCont( int signum ) {
			delete Server::serverConsole;
			serverConsole = new ServerConsole();
		}
#endif

		ServerConsole::ServerConsole() {
			std::memset( &input, 0, sizeof(input) );
#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
			signal( SIGTTIN, SIG_IGN );
			signal( SIGTTOU, SIG_IGN );

			signal( SIGCONT, SigCont );
			fcntl( STDIN_FILENO, F_SETFL, fcntl( STDIN_FILENO, F_GETFL, 0 ) | O_NONBLOCK );

			tcgetattr( STDIN_FILENO, &tc );
			ec_erase = tc.c_cc[VERASE];
			ec_eof = tc.c_cc[VEOF];

			tc.c_lflag &= ~(ECHO | ICANON);
			tc.c_iflag &= ~(ISTRIP | INPCK);
			tc.c_cc[VMIN] = 1;
			tc.c_cc[VTIME] = 0;
			tcsetattr( STDIN_FILENO, TCSADRAIN, &tc );
#elif defined(XS_OS_WINDOWS)
			AllocConsole();

			// set the screen buffer to be big enough to let us scroll text
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &info );
			info.dwSize.Y = 20;

			SetConsoleScreenBufferSize( GetStdHandle( STD_OUTPUT_HANDLE ), info.dwSize );

			// redirect unbuffered STDOUT to the console
			hOutput = GetStdHandle( STD_OUTPUT_HANDLE );
			int dOutput = _open_osfhandle( reinterpret_cast<intptr_t>( hOutput ), _O_TEXT );
			FILE *fOutput = _fdopen( dOutput, "w" );
			*stdout = *fOutput;
			setvbuf( stdout, NULL, _IONBF, 0 );

			// redirect unbuffered STDIN to the console
			hInput = GetStdHandle( STD_INPUT_HANDLE );
			int dInput = _open_osfhandle( reinterpret_cast<intptr_t>( hInput ), _O_TEXT );
			FILE *fInput = _fdopen( dInput, "r" );
			*stdin = *fInput;
			setvbuf( stdin, NULL, _IONBF, 0 );

			// redirect unbuffered STDERR to the console
			hError = GetStdHandle( STD_ERROR_HANDLE );
			int dError = _open_osfhandle( reinterpret_cast<intptr_t>( hError ), _O_TEXT );
			FILE *fError = _fdopen( dError, "w" );
			*stderr = *fError;
			setvbuf( stderr, NULL, _IONBF, 0 );

			// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
			//std::ios::sync_with_stdio();

			// allow mouse wheel scrolling
			DWORD origMode;
			GetConsoleMode( hInput, &origMode );
			SetConsoleMode( hInput, origMode & ~ENABLE_MOUSE_INPUT );

			FlushConsoleInputBuffer( hInput );
			GetConsoleScreenBufferInfo( hOutput, &info );

			SetConsoleTitle( WINDOW_TITLE " Dedicated Server" );
#endif
		}

		void ServerConsole::FlushInput( void ) const {
#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
			char key;
			do {
				// ...
			} while ( read( STDIN_FILENO, &key, 1 ) != -1 );
#endif
		}

		void ServerConsole::Backspace( void ) const {
#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
			static char keys[] = { '\b', ' ', '\b' };
			for ( auto &key : keys ) {
				write( STDOUT_FILENO, &key, 1 );
			}
#endif
		}

#if defined(XS_OS_WINDOWS)
		void ServerConsole::Show( void ) {
			CONSOLE_SCREEN_BUFFER_INFO binfo;
			COORD writeSize = { bufferSize, 1 };
			COORD writePos = { 0, 0 };
			SMALL_RECT writeArea = { 0, 0, 0, 0 };
			COORD cursorPos;
			CHAR_INFO line[bufferSize];

			GetConsoleScreenBufferInfo( hOutput, &binfo );

			writeArea.Left = 0;
			writeArea.Top = binfo.dwCursorPosition.Y;
			writeArea.Bottom = binfo.dwCursorPosition.Y;
			writeArea.Right = bufferSize;

			for ( size_t i = 0; i < bufferSize; i++ ) {
				if ( i < input.head ) {
					line[i].Char.AsciiChar = input.buffer[i];
				}
				else {
					line[i].Char.AsciiChar = ' ';
				}
				line[i].Attributes = binfo.wAttributes; //TODO: colour
			}

			WriteConsoleOutput( hOutput, line, writeSize, writePos, &writeArea );

			// set cursor position
			cursorPos.Y = binfo.dwCursorPosition.Y;
			SDL_assert( input.head < INT16_MAX );
			cursorPos.X = input.head < bufferSize
							? static_cast<SHORT>( input.head )
							: (bufferSize > binfo.srWindow.Right)
								? binfo.srWindow.Right
								: bufferSize;
			SetConsoleCursorPosition( hOutput, cursorPos );
		}
#endif

		const char *ServerConsole::Read( void ) {
			static char text[bufferSize];
#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
			char key;
			int avail = read( STDIN_FILENO, &key, 1 );
			if ( avail != -1 ) {
				//FIXME: what are these magic numbers?
				if ( key == ec_erase || key == 127 || key == 8 ) {
					if ( input.head > 0u ) {
						input.buffer[--input.head] = '\0';
						Backspace();
					}
					return nullptr;
				}
				if ( key < 0x20 ) {
					// control character
					if ( key == '\n' ) {
						//TODO: add to history
						String::Copy( text, input.buffer, sizeof(text) );

						// clear input line
						std::memset( input.buffer, 0, sizeof(input.buffer) );
						input.head = 0u;

						write( STDOUT_FILENO, &key, 1 );
						write( STDOUT_FILENO, promptText, strlen( promptText ) );

						return text;
					}
					else if ( key == '\t' ) {
						//TODO: autocomplete
						return nullptr;
					}
					avail = read( STDIN_FILENO, &key, 1 );
					if ( avail != -1 ) {
						// vt100 keys
						if ( key == '[' || key == 'O' ) {
							avail = read( STDIN_FILENO, &key, 1 );
							if ( avail != -1 ) {
								switch ( key ) {

								case 'A': {
									//TODO: get previous history
									FlushInput();
									return nullptr;
								} break;

								case 'B': {
									//TODO: get next history
									FlushInput();
									return nullptr;
								} break;

								case 'C':
								case 'D': {
									return nullptr;
								} break;

								default: {
									// ...
								} break;

								}
							}
						}
					}

					console.Print( PrintLevel::Debug, "Dropping ISCTL sequence: %d, TTY_erase: %d\n",
						key,
						ec_erase
					);
					FlushInput();
					return nullptr;
				}

				input.buffer[input.head++] = key;
				if ( input.head >= sizeof(input.buffer) - 1 ) {
					input.head = sizeof(input.buffer) - 1;
				}

				write( STDOUT_FILENO, &key, 1 );
			}
#elif defined(XS_OS_WINDOWS)
			DWORD events = 0;
			if ( !GetNumberOfConsoleInputEvents( hInput, &events ) ) {
				return nullptr;
			}

			if ( events < 1 ) {
				return nullptr;
			}

			INPUT_RECORD buff[bufferSize];
			// if we have overflowed, start dropping oldest input events
			if ( events >= ARRAY_LEN(buff) ) {
				ReadConsoleInput( hInput, buff, 1, &events );
				return nullptr;
			}

			DWORD count = 0;
			if ( !ReadConsoleInput( hInput, buff, events, &count ) ) {
				return nullptr;
			}

			FlushConsoleInputBuffer( hInput );

			for ( size_t i = 0u; i < count; i++ ) {
				if ( buff[i].EventType != KEY_EVENT ) {
					continue;
				}

				if ( !buff[i].Event.KeyEvent.bKeyDown ) {
					continue;
				}

				bool keyHandled = true;
				WORD key = buff[i].Event.KeyEvent.wVirtualKeyCode;
				switch ( key ) {
				case VK_RETURN: {
					String::Copy( text, input.buffer, sizeof(text) );

					// re-initialise the input buffer
					input.head = 0;
					std::memset( input.buffer, 0, sizeof(input.buffer) );

					fprintf( stdout, "\n" );
					return text;
				} break;

				case VK_UP: {
					//TODO: previous history
				} break;

				case VK_DOWN: {
					//TODO: next history
				} break;

				case VK_LEFT: {
					/*
					input.head--;
					if ( input.head < 0 ) {
						input.head = 0;
					}
					*/
				} break;

				case VK_RIGHT: {
					/*
					input.head++;
					if ( input.head >= bufferSize ) {
						input.head = bufferSize - 1;
					}
					*/
				} break;

				case VK_HOME: {
					//input.head = 0;
				} break;

				case VK_END: {
					//input.head = bufferSize - 1;
				} break;

				case VK_TAB: {
					//TODO: autocomplete
				} break;

				default: {
					keyHandled = false;
				} break;

				}

				if ( keyHandled ) {
					break;
				}

				char c = buff[i].Event.KeyEvent.uChar.AsciiChar;

				if ( key == VK_BACK ) {
					if ( input.head > 0u ) {
						input.buffer[--input.head] = '\0';
					}
				}
				else if ( c ) {
					if ( input.head < sizeof(input.buffer) - 1 ) {
						input.buffer[input.head++] = c;
						SDL_assert( input.head != sizeof(input.buffer) );
						input.buffer[input.head] = '\0';
					}
				}
			}

			Show();
#endif
			return nullptr;
		}

		ServerConsole::~ServerConsole() {
#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
			tcsetattr( STDIN_FILENO, TCSADRAIN, &tc );
			fcntl( STDIN_FILENO, F_SETFL, fcntl( STDIN_FILENO, F_GETFL, 0 ) & ~O_NONBLOCK );
#endif
		}

	} // namespace Server

} // namespace XS
