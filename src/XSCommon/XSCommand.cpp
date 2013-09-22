#include "XSSystem/XSInclude.h"

#include "XSCommon/XSCommand.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSCvar.h"

namespace XS {

	namespace Command {

		// commands
		static std::unordered_map<std::string, commandFunc_t> commands;

		// command context (i.e. args)
		commandContext *context = NULL;

		// command buffer
		static std::vector<std::string> buffer;


		static void Cmd_SetCvar( const commandContext *context ) {
			Cvar::Create( context->args[1], context->args[2] );
		}

		void Init( void ) {
			AddCommand( "set", Cmd_SetCvar );
		}

		// command buffer

		void Append( const char *cmd ) {
			buffer.push_back( cmd );
		}

		void ExecuteBuffer( void ) {
			for ( auto it = buffer.begin(); it != buffer.end(); ++it ) {
				context = new commandContext;

				context->args = String::Split( *it, ' ' );

				if ( context->args.size() == 0 )
					continue;

				commandFunc_t func = commands[context->args[0]];
				if ( func ) {
					func( context );
					continue;
				}

				delete context;
			}
			buffer.clear();
		}

		// commands

		bool AddCommand( const char *name, commandFunc_t cmd ) {
			commandFunc_t &func = commands[name];
			if ( func )
				return false;

			func = cmd;
			return true;
		}

	} // namespace Command

} // namespace XS
