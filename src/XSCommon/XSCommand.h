#pragma once

#include <vector>
#include <string>

namespace XS {

	typedef std::vector<std::string> commandContext_t;

	namespace Command {

		typedef void (*commandFunc_t)( const commandContext_t * const context );

		void Init( void );

		bool AddCommand( const char *name, commandFunc_t cmd );

		void Append( const char *str, char delimiter = ';' );
		void ExecuteBuffer( void );

	} // namespace Command

} // namespace XS
