#pragma once

namespace XS {

	struct commandContext_t {
		std::vector<std::string> args;
	};

	namespace Command {

		typedef void (*commandFunc_t)( const commandContext_t *context );

		void Init( void );

		bool AddCommand( const char *name, commandFunc_t cmd );

		void Append( const char *cmd );
		void ExecuteBuffer( void );

	} // namespace Command

} // namespace XS
