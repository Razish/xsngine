#include <queue>
#include <map>

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
		static std::unordered_map<std::string, CommandFunc> commandTable;

		// command buffer
		static std::queue<std::string> buffer;

		static void Cmd_ClearConsole( const CommandContext * const context ) {
			console.Clear();
		}

		static void Cmd_ListCommands( const CommandContext * const context ) {
			console.Print( PrintLevel::Normal, "Listing commands...\n" );
			Indent indent( 1 );

			std::map<std::string, CommandFunc> sorted( commandTable.begin(), commandTable.end() );
			for ( const auto &cmd : sorted ) {
				console.Print( PrintLevel::Normal, "%s\n", cmd.first.c_str() );
			}
		}

		static void Cmd_ListCvars( const CommandContext * const context ) {
			Cvar::List();
		}

		static void Cmd_PrintCvar( const CommandContext * const context ) {
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

		static void Cmd_SetCvar( const CommandContext * const context ) {
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

		static void Cmd_ToggleCvar( const CommandContext * const context ) {
			Cvar *cv = Cvar::Get( (*context)[0] );

			cv->Set( !cv->GetBool() );
		}

		static void Cmd_Quit( const CommandContext * const context ) {
			throw( XSError( "Quit application" ) );
		}

		void Init( void ) {
			AddCommand( "bind", Client::Cmd_SetBind );
			AddCommand( "bindlist", Client::Cmd_ListBinds );
			AddCommand( "clear", Cmd_ClearConsole );
			AddCommand( "cmdlist", Cmd_ListCommands );
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
			//FIXME: strip whitespace? seems to work fine
			for ( const auto &command : commands ) {
				buffer.push( command );
			}
		}

		void ExecuteBuffer( void ) {
			while ( !buffer.empty() ) {
				const auto &cmd = buffer.front();

				// tokenise the arguments
				const char delim = ' ';
				size_t start = cmd.find( delim );
				std::string name = cmd.substr( 0, start );

				CommandContext context;
				if ( start != std::string::npos && start != cmd.size() - 1 ) {
					context = String::Split( &cmd[start + 1], ' ' );

					// strip any quotes around the arguments
					for ( auto &tok : context ) {
						tok.erase( std::remove( tok.begin(), tok.end(), '"' ), tok.end() );
					}
				}

				const CommandFunc &func = commandTable[name];
				if ( func ) {
					func( &context );
				}
				else {
					console.Print( PrintLevel::Normal, "Unknown command \"%s\"\n", cmd.c_str()  );
				}

				buffer.pop();
			}
		}

		// commands

		bool AddCommand( const char *name, CommandFunc cmd ) {
			CommandFunc &func = commandTable[name];
			if ( func ) {
				console.Print( PrintLevel::Normal, "Tried to register command \"%s\" twice\n", name );
				return false;
			}

			func = cmd;
			return true;
		}

		bool AddButton( const char *name, CommandFunc down, CommandFunc up ) {
			CommandFunc &downFunc = commandTable[String::Format( "+%s", name ).c_str()];
			if ( downFunc ) {
				console.Print( PrintLevel::Normal, "Tried to register command \"%s\" twice\n", name );
				return false;
			}
			downFunc = down;

			CommandFunc &upFunc = commandTable[String::Format( "-%s", name ).c_str()];
			if ( upFunc ) {
				console.Print( PrintLevel::Normal, "Tried to register command \"%s\" twice\n", name );
				return false;
			}
			upFunc = up;

			return true;
		}

	} // namespace Command

} // namespace XS
