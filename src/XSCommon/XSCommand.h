#pragma once

namespace XS {

	typedef std::vector<std::string> commandContext_t;

	namespace Command {

		typedef void (*commandFunc_t)( const commandContext_t *context );

		void Init( void );

		bool AddCommand( const char *name, commandFunc_t cmd );

		void Append( const char *cmd );
		void ExecuteBuffer( void );

	} // namespace Command

} // namespace XS
