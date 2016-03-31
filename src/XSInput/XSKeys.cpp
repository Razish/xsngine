#include <map>

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_keyboard.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSEvent.h"
#include "XSInput/XSKeys.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSClientConsole.h"

namespace XS {

	namespace Client {

		std::unordered_map<SDL_Keycode, bool> keystate;
		static std::unordered_map<SDL_Keycode, std::string> binds;

		struct KeyMap {
			const char *name;
			char printable;
			char shifted;
			SDL_Keycode keycode;
		};

		// map xsn keynames to SDL keycodes
		//TODO: unordered map?
		static const KeyMap keymap[] = {
			{ "RETURN",			'\0',	'\0',	SDLK_RETURN },
			{ "ESCAPE",			'\0',	'\0',	SDLK_ESCAPE },
			{ "BACKSPACE",		'\0',	'\0',	SDLK_BACKSPACE },
			{ "TAB",			'\t',	'\0',	SDLK_TAB },
			{ "SPACE",			' ',	'\0',	SDLK_SPACE },
			{ "QUOTE",			'\'',	'"',	SDLK_QUOTE },
			{ "COMMA",			',',	'<',	SDLK_COMMA },
			{ "MINUS",			'-',	'_',	SDLK_MINUS },
			{ "PERIOD",			'.',	'>',	SDLK_PERIOD },
			{ "SLASH",			'/',	'?',	SDLK_SLASH },
			{ "0",				'0',	')',	SDLK_0 },
			{ "1",				'1',	'!',	SDLK_1 },
			{ "2",				'2',	'@',	SDLK_2 },
			{ "3",				'3',	'#',	SDLK_3 },
			{ "4",				'4',	'$',	SDLK_4 },
			{ "5",				'5',	'%',	SDLK_5 },
			{ "6",				'6',	'^',	SDLK_6 },
			{ "7",				'7',	'&',	SDLK_7 },
			{ "8",				'8',	'*',	SDLK_8 },
			{ "9",				'9',	'(',	SDLK_9 },
			{ ";",				';',	':',	SDLK_SEMICOLON },
			{ "=",				'=',	'+',	SDLK_EQUALS },
			{ "[",				'[',	'{',	SDLK_LEFTBRACKET },
			{ "\\",				'\\',	'|',	SDLK_BACKSLASH },
			{ "]",				']',	'}',	SDLK_RIGHTBRACKET },
			{ "`",				'`',	'~',	SDLK_BACKQUOTE },
			{ "a",				'a',	'A',	SDLK_a },
			{ "b",				'b',	'B',	SDLK_b },
			{ "c",				'c',	'C',	SDLK_c },
			{ "d",				'd',	'D',	SDLK_d },
			{ "e",				'e',	'E',	SDLK_e },
			{ "f",				'f',	'F',	SDLK_f },
			{ "g",				'g',	'G',	SDLK_g },
			{ "h",				'h',	'H',	SDLK_h },
			{ "i",				'i',	'I',	SDLK_i },
			{ "j",				'j',	'J',	SDLK_j },
			{ "k",				'k',	'K',	SDLK_k },
			{ "l",				'l',	'L',	SDLK_l },
			{ "m",				'm',	'M',	SDLK_m },
			{ "n",				'n',	'N',	SDLK_n },
			{ "o",				'o',	'O',	SDLK_o },
			{ "p",				'p',	'P',	SDLK_p },
			{ "q",				'q',	'Q',	SDLK_q },
			{ "r",				'r',	'R',	SDLK_r },
			{ "s",				's',	'S',	SDLK_s },
			{ "t",				't',	'T',	SDLK_t },
			{ "u",				'u',	'U',	SDLK_u },
			{ "v",				'v',	'V',	SDLK_v },
			{ "w",				'w',	'W',	SDLK_w },
			{ "x",				'x',	'X',	SDLK_x },
			{ "y",				'y',	'Y',	SDLK_y },
			{ "z",				'z',	'Z',	SDLK_z },
			{ "CAPSLOCK",		'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_CAPSLOCK ) },
			{ "F1",				'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F1 ) },
			{ "F2",				'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F2 ) },
			{ "F3",				'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F3 ) },
			{ "F4",				'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F4 ) },
			{ "F5",				'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F5 ) },
			{ "F6",				'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F6 ) },
			{ "F7",				'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F7 ) },
			{ "F8",				'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F8 ) },
			{ "F9",				'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F9 ) },
			{ "F10",			'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F10 ) },
			{ "F11",			'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F11 ) },
			{ "F12",			'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F12 ) },
			{ "PRINTSCREEN",	'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_PRINTSCREEN ) },
			{ "SCROLLOCK",		'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_SCROLLLOCK ) },
			{ "PAUSE",			'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_PAUSE ) },
			{ "INSERT",			'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_INSERT ) },
			{ "HOME",			'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_HOME ) },
			{ "PAGEUP",			'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_PAGEUP ) },
			{ "DELETE",			'\0',	'\0',	SDLK_DELETE },
			{ "END",			'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_END ) },
			{ "PAGEDOWN",		'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_PAGEDOWN ) },
			{ "RIGHT",			'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_RIGHT ) },
			{ "LEFT",			'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_LEFT ) },
			{ "DOWN",			'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_DOWN ) },
			{ "UP",				'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_UP ) },
			{ "NUMLOCK",		'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_NUMLOCKCLEAR ) },
			{ "KP_DIVIDE",		'/',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_DIVIDE ) },
			{ "KP_MULTIPLY",	'*',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_MULTIPLY ) },
			{ "KP_MINUS",		'-',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_MINUS ) },
			{ "KP_PLUS",		'+',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_PLUS ) },
			{ "KP_ENTER",		'\0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_ENTER ) },
			{ "KP_1",			'1',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_1 ) },
			{ "KP_2",			'2',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_2 ) },
			{ "KP_3",			'3',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_3 ) },
			{ "KP_4",			'4',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_4 ) },
			{ "KP_5",			'5',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_5 ) },
			{ "KP_6",			'6',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_6 ) },
			{ "KP_7",			'7',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_7 ) },
			{ "KP_8",			'8',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_8 ) },
			{ "KP_9",			'9',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_9 ) },
			{ "KP_0",			'0',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_0 ) },
			{ "KP_PERIOD",		'.',	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_PERIOD ) },
			{ "LCTRL",			'\0',	'\0',	SDLK_LCTRL },
			{ "LSHIFT",			'\0',	'\0',	SDLK_LSHIFT },
			{ "LALT",			'\0',	'\0',	SDLK_LALT },
			{ "LGUI",			'\0',	'\0',	SDLK_LGUI },
			{ "RCTRL",			'\0',	'\0',	SDLK_RCTRL },
			{ "RSHIFT",			'\0',	'\0',	SDLK_RSHIFT },
			{ "RALT",			'\0',	'\0',	SDLK_RALT },
			{ "RGUI",			'\0',	'\0',	SDLK_RGUI },
		};

		static SDL_Keycode GetKeycodeForName( const char *name ) {
			for ( const KeyMap &km : keymap ) {
				if ( !String::Compare( name, km.name ) ) {
					return km.keycode;
				}
			}
			return SDLK_UNKNOWN;
		}

		static const char *GetNameForKeycode( SDL_Keycode keycode ) {
			for ( const KeyMap &km : keymap ) {
				if ( keycode == km.keycode ) {
					return km.name;
				}
			}
			return nullptr;
		}

		char GetPrintableCharForKeycode( SDL_Keycode keycode ) {
			for ( const KeyMap &km : keymap ) {
				if ( km.keycode == keycode ) {
					// return the actual printable character, taking into account shift + capslock
					bool shift = false;

					if ( keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT] ) {
						shift = !shift;
					}

					const bool capsLock = !!(SDL_GetModState() & KMOD_CAPS);
					if ( capsLock && km.keycode >= 'a' && km.keycode <= 'z' ) {
						shift = !shift;
					}

					return shift ? km.shifted : km.printable;
				}
			}
			return '\0';
		}

		void Cmd_ListBinds( const CommandContext &context ) {
			console.Print( PrintLevel::Normal, "Listing binds...\n" );

			std::map<SDL_Keycode, std::string> sorted( binds.begin(), binds.end() );

			Indent indent( 1 );
			for ( const auto &bind : sorted ) {
				if ( bind.second.empty() ) {
					continue;
				}

				console.Print( PrintLevel::Normal, "%-8s: %s\n", GetNameForKeycode( bind.first ), bind.second.c_str() );
			}
		}

		void Cmd_SetBind( const CommandContext &context ) {
			if ( context.size() < 1 ) {
				console.Print( PrintLevel::Normal, "Usage: bind <key> [command]\n" );
				return;
			}

			SDL_Keycode keycode = GetKeycodeForName( context[0].c_str() );

			if ( keycode == SDLK_UNKNOWN ) {
				console.Print( PrintLevel::Normal, "Unknown key \"%s\"", context[0].c_str() );
				return;
			}

			if ( context.size() == 1 ) {
				std::string boundString = "";
				if ( !binds[keycode].empty() ) {
					boundString = binds[keycode];
				}
				console.Print( PrintLevel::Normal, "%s: \"%s\"\n", context[0].c_str(), boundString.c_str() );
				return;
			}

			// join all args, so we can do /bind x command argument
			const size_t size = context.size();
			std::string value;
			for ( size_t i = 1; i < size; i++ ) {
				value += context[i];
				if ( i != size - 1 ) {
					value += " ";
				}
			}

			binds[keycode] = value;
		}

		void ExecuteBind( const struct KeyboardEvent &ev ) {
			const std::string &cmd = binds[ev.key];

			if ( cmd.empty() ) {
				if ( ev.down ) {
					console.Print( PrintLevel::Developer, "ExecuteBind: \"%s\" (%d) is not bound\n",
						GetNameForKeycode( ev.key ),
						ev.key
					);
				}
				return;
			}

			if ( cmd[0] == '+' ) {
				//TODO: handle buttons?
				if ( ev.down ) {
					Command::Append( cmd.c_str() );
				}
				else {
					std::string newCmd = cmd;
					newCmd[0] = '-';
					Command::Append( newCmd.c_str() );
				}
				return;
			}

			if ( ev.down ) {
				Command::Append( cmd.c_str() );
			}
		}

		void WriteBinds( std::string &str ) {
			std::map<SDL_Keycode, std::string> sorted( binds.begin(), binds.end() );

			for ( const auto &it : sorted ) {
				SDL_Keycode key = it.first;
				const std::string &cmd = it.second;
				if ( !cmd.empty() ) {
					str += String::Format( "bind %s \"%s\"\n", GetNameForKeycode( key ), cmd.c_str() );
				}
			}
		}

	} // namespace Client

} // namespace XS
