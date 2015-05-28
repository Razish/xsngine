#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#endif

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSServer/XSServerConsole.h"
#include "XSServer/XSServer.h"

namespace XS {

	namespace Server {
		const char *ServerConsole::promptText = "tty] ";
#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
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

		const char *ServerConsole::Read( void ) {
#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
			static char text[1024];
			int avail;
			char key;
			size_t size;

			avail = read( STDIN_FILENO, &key, 1 );
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

						size = write( STDOUT_FILENO, &key, 1 );
						size = write( STDOUT_FILENO, promptText, strlen( promptText ) );

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

					console.Print( PrintLevel::Debug,
						"Dropping ISCTL sequence: %d, TTY_erase: %d\n",
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

				size = write( STDOUT_FILENO, &key, 1 );
			}
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
