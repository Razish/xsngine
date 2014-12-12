#include <map>

#include <SDL2/SDL_keycode.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSInput/XSKeys.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSClientConsole.h"

namespace XS {

	namespace Client {

		std::unordered_map<SDL_Keycode, bool> keystate;
		static std::unordered_map<SDL_Keycode, std::string> binds;

		struct keyMap_t {
			const char *name;
			char printable;
			SDL_Keycode keycode;
		};

		// map xsn keynames to SDL keycodes
		//TODO: unordered map?
		static const keyMap_t keymap[] = {
			{ "RETURN",			'\0',	SDLK_RETURN },
			{ "ESCAPE",			'\0',	SDLK_ESCAPE },
			{ "BACKSPACE",		'\0',	SDLK_BACKSPACE },
			{ "TAB",			'\t',	SDLK_TAB },
			{ "SPACE",			' ',	SDLK_SPACE },
			{ "QUOTE",			'\'',	SDLK_QUOTE },
			{ "COMMA",			',',	SDLK_COMMA },
			{ "MINUS",			'-',	SDLK_MINUS },
			{ "PERIOD",			'.',	SDLK_PERIOD },
			{ "SLASH",			'/',	SDLK_SLASH },
			{ "0",				'0',	SDLK_0 },
			{ "1",				'1',	SDLK_1 },
			{ "2",				'2',	SDLK_2 },
			{ "3",				'3',	SDLK_3 },
			{ "4",				'4',	SDLK_4 },
			{ "5",				'5',	SDLK_5 },
			{ "6",				'6',	SDLK_6 },
			{ "7",				'7',	SDLK_7 },
			{ "8",				'8',	SDLK_8 },
			{ "9",				'9',	SDLK_9 },
			{ ";",				';',	SDLK_SEMICOLON },
			{ "=",				'=',	SDLK_EQUALS },
			{ "[",				'[',	SDLK_LEFTBRACKET },
			{ "\\",				'\\',	SDLK_BACKSLASH },
			{ "]",				']',	SDLK_RIGHTBRACKET },
			{ "`",				'`',	SDLK_BACKQUOTE },
			{ "a",				'a',	SDLK_a },
			{ "b",				'b',	SDLK_b },
			{ "c",				'c',	SDLK_c },
			{ "d",				'd',	SDLK_d },
			{ "e",				'e',	SDLK_e },
			{ "f",				'f',	SDLK_f },
			{ "g",				'g',	SDLK_g },
			{ "h",				'h',	SDLK_h },
			{ "i",				'i',	SDLK_i },
			{ "j",				'j',	SDLK_j },
			{ "k",				'k',	SDLK_k },
			{ "l",				'l',	SDLK_l },
			{ "m",				'm',	SDLK_m },
			{ "n",				'n',	SDLK_n },
			{ "o",				'o',	SDLK_o },
			{ "p",				'p',	SDLK_p },
			{ "q",				'q',	SDLK_q },
			{ "r",				'r',	SDLK_r },
			{ "s",				's',	SDLK_s },
			{ "t",				't',	SDLK_t },
			{ "u",				'u',	SDLK_u },
			{ "v",				'v',	SDLK_v },
			{ "w",				'w',	SDLK_w },
			{ "x",				'x',	SDLK_x },
			{ "y",				'y',	SDLK_y },
			{ "z",				'z',	SDLK_z },
			{ "CAPSLOCK",		'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_CAPSLOCK ) },
			{ "F1",				'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F1 ) },
			{ "F2",				'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F2 ) },
			{ "F3",				'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F3 ) },
			{ "F4",				'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F4 ) },
			{ "F5",				'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F5 ) },
			{ "F6",				'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F6 ) },
			{ "F7",				'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F7 ) },
			{ "F8",				'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F8 ) },
			{ "F9",				'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F9 ) },
			{ "F10",			'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F10 ) },
			{ "F11",			'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F11 ) },
			{ "F12",			'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_F12 ) },
			{ "PRINTSCREEN",	'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_PRINTSCREEN ) },
			{ "SCROLLOCK",		'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_SCROLLLOCK ) },
			{ "PAUSE",			'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_PAUSE ) },
			{ "INSERT",			'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_INSERT ) },
			{ "HOME",			'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_HOME ) },
			{ "PAGEUP",			'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_PAGEUP ) },
			{ "DELETE",			'\0',	SDLK_DELETE },
			{ "END",			'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_END ) },
			{ "PAGEDOWN",		'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_PAGEDOWN ) },
			{ "RIGHT",			'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_RIGHT ) },
			{ "LEFT",			'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_LEFT ) },
			{ "DOWN",			'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_DOWN ) },
			{ "UP",				'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_UP ) },
			{ "NUMLOCK",		'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_NUMLOCKCLEAR ) },
			{ "KP_DIVIDE",		'/',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_DIVIDE ) },
			{ "KP_MULTIPLY",	'*',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_MULTIPLY ) },
			{ "KP_MINUS",		'-',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_MINUS ) },
			{ "KP_PLUS",		'+',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_PLUS ) },
			{ "KP_ENTER",		'\0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_ENTER ) },
			{ "KP_1",			'1',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_1 ) },
			{ "KP_2",			'2',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_2 ) },
			{ "KP_3",			'3',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_3 ) },
			{ "KP_4",			'4',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_4 ) },
			{ "KP_5",			'5',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_5 ) },
			{ "KP_6",			'6',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_6 ) },
			{ "KP_7",			'7',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_7 ) },
			{ "KP_8",			'8',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_8 ) },
			{ "KP_9",			'9',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_9 ) },
			{ "KP_0",			'0',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_0 ) },
			{ "KP_PERIOD",		'.',	SDL_SCANCODE_TO_KEYCODE( SDL_SCANCODE_KP_PERIOD ) },
		};
		static const size_t keymapSize = ARRAY_LEN( keymap );

		static SDL_Keycode GetKeycodeForName( const char *name ) {
			const keyMap_t *km = nullptr;
			size_t i = 0;

			for ( i = 0, km = keymap; i < keymapSize; i++, km++ ) {
				if ( !String::Compare( name, km->name ) ) {
					return km->keycode;
				}
			}
			return SDLK_UNKNOWN;
		}

		static const char *GetNameForKeycode( SDL_Keycode keycode ) {
			const keyMap_t *km = nullptr;
			size_t i = 0;

			for ( i = 0, km = keymap; i < keymapSize; i++, km++ ) {
				if ( keycode == km->keycode ) {
					return km->name;
				}
			}
			return nullptr;
		}

		char GetPrintableCharForKeycode( SDL_Keycode keycode ) {
			const keyMap_t *km = NULL;
			size_t i = 0;

			for ( i = 0, km = keymap; i < keymapSize; i++, km++ ) {
				if ( km->keycode == keycode ) {
					return km->printable;
				}
			}
			return '\0';
		}

		void Cmd_ListBinds( const commandContext_t * const context ) {
			console.Print( "Listing binds...\n" );

			std::map<SDL_Keycode, std::string> sorted( binds.begin(), binds.end() );

			Indent indent( 1 );
			for ( const auto &bind : sorted ) {
				if ( bind.second.empty() ) {
					continue;
				}

				console.Print( "%-8s: %s\n", GetNameForKeycode( bind.first ), bind.second.c_str() );
			}
		}

		void Cmd_SetBind( const commandContext_t * const context ) {
			if ( context->size() < 2 ) {
				console.Print( "\"bind\" failed. Must specify a key and command\n" );
				return;
			}

			SDL_Keycode keycode = GetKeycodeForName( (*context)[0].c_str() );

			if ( keycode == SDLK_UNKNOWN ) {
				console.Print( "\"bind\" failed. Unknown key \"%s\"\n", (*context)[0].c_str() );
				return;
			}

			// join all args, so we can do /bind x command argument
			const size_t size = context->size();
			std::string value;
			for ( size_t i = 1; i < size; i++ ) {
				value += (*context)[i];
				if ( i != size-1 ) {
					value += " ";
				}
			}

			binds[keycode] = value;
		}

		static void ExecuteBind( SDL_Keycode key, bool down ) {
			const std::string &cmd = binds[key];

			if ( cmd.empty() ) {
				/*
				if ( down ) {
					console.Print( "ExecuteBind: \"%s\" (%d) is not bound\n", GetNameForKeycode( key ), key );
				}
				*/
				return;
			}

			if ( cmd[0] == '+' ) {
				//TODO: handle buttons?
				return;
			}

			if ( down ) {
				Command::Append( cmd.c_str() );
			}
		}

		void KeyEvent( SDL_Keycode key, bool down ) {
			if ( clientConsole && clientConsole->KeyEvent( key, down ) ) {
				return;
			}

			/*
			if ( XSClientGame->KeyEvent( key, down ) ) {
				return;
			}
			*/

			ExecuteBind( key, down );
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
