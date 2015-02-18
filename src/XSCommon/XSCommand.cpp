#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSError.h"
#include "XSInput/XSKeys.h"
#include "XSClient/XSClient.h"

namespace XS {

	namespace Command {

		// commands
		static std::unordered_map<std::string, commandFunc_t> commandTable;

		// command buffer
		static std::vector<std::string> buffer;

		static void Cmd_ListCvars( const commandContext_t * const context ) {
			Cvar::List();
		}

		static void Cmd_PrintCvar( const commandContext_t * const context ) {
			if ( context->size() < 1 ) {
				console.Print( PrintLevel::Normal, "\"print\" failed. Must specify at-least one cvar\n" );
				return;
			}

			for ( const auto &it : *context ) {
				const Cvar *cv = Cvar::Get( it );
				if ( cv ) {
					console.Print( PrintLevel::Normal, "%s: \"%s\"\n", it.c_str(), cv->GetFullCString() );
				}
				else {
					console.Print( PrintLevel::Normal, "%s: does not exist\n", it.c_str() );
				}
			}
		}

		static void Cmd_SetCvar( const commandContext_t * const context ) {
			if ( context->size() < 2 ) {
				console.Print( PrintLevel::Normal, "\"set\" failed. Must specify a cvar and value\n" );
				return;
			}

			Cvar *cv = Cvar::Create( (*context)[0] );

			size_t size = context->size();
			std::string value;
			for ( size_t i = 1; i < size; i++ ) {
				value += (*context)[i];
				if ( i != size - 1 ) {
					value += " ";
				}
			}
			cv->Set( value );
		}

		static void Cmd_ToggleCvar( const commandContext_t * const context ) {
			Cvar *cv = Cvar::Get( (*context)[0] );

			cv->Set( !cv->GetBool() );
		}

		static void Cmd_Quit( const commandContext_t * const context ) {
			throw( XSError( "Quit application" ) );
		}

		void Init( void ) {
			AddCommand( "bind", Client::Cmd_SetBind );
			AddCommand( "bindlist", Client::Cmd_ListBinds );
			AddCommand( "cvarlist", Cmd_ListCvars );
			AddCommand( "print", Cmd_PrintCvar );
			AddCommand( "set", Cmd_SetCvar );
			AddCommand( "toggle", Cmd_ToggleCvar );
			AddCommand( "toggleconsole", Client::Cmd_ToggleConsole );
			AddCommand( "quit", Cmd_Quit );
		}

		// command buffer

		void Append( const char *str, char delimiter ) {
			std::vector<std::string> commands = String::Split( str, delimiter );
			for ( const auto &command : commands ) {
				buffer.push_back( command );
			}
		}

		void ExecuteBuffer( void ) {
			for ( const auto &it : buffer ) {
				commandContext_t context;

				// tokenise the arguments
				const char delim = ' ';
				size_t start = it.find( delim );
				std::string name = it.substr( 0, start );

				if ( start != std::string::npos && start != it.size() - 1 ) {
					context = String::Split( &it[start + 1], ' ' );

					// strip any quotes around the arguments
					for ( auto &tok : context ) {
						tok.erase( std::remove( tok.begin(), tok.end(), '"' ), tok.end() );
					}
				}

				const commandFunc_t &func = commandTable[name];
				if ( func ) {
					func( &context );
					continue;
				}
				else {
					console.Print( PrintLevel::Normal, "Unknown command \"%s\"\n", it.c_str()  );
				}
			}
			buffer.clear();
		}

		// commands

		bool AddCommand( const char *name, commandFunc_t cmd ) {
			commandFunc_t &func = commandTable[name];
			if ( func ) {
				console.Print( PrintLevel::Normal, "Tried to register command \"%s\" twice\n", name );
				return false;
			}

			func = cmd;
			return true;
		}

	} // namespace Command

} // namespace XS
