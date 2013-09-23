#pragma once

namespace XS {

	namespace Command {
		
		struct commandContext {
			std::vector<std::string> args;
		};

		typedef void (*commandFunc_t)( const commandContext *context );

		void Init( void );

		bool AddCommand( const char *name, commandFunc_t cmd );

		void Append( const char *cmd );
		void ExecuteBuffer( void );

	} // namespace Command

} // namespace XS
