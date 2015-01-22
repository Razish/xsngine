#pragma once

#include <vector>
#include <string>

namespace XS {

	typedef std::vector<std::string> commandContext_t;

	namespace Command {

		typedef void (*commandFunc_t)( const commandContext_t * const context );

		// initialise the command system
		void Init(
			void
		);

		// associate a console command with a function
		bool AddCommand(
			const char *name,
			commandFunc_t cmd
		);

		// append text to the command buffer to be executed in a batch
		void Append(
			const char *str,
			char delimiter = ';'
		);

		// execute queued commands in the buffer
		void ExecuteBuffer(
			void
		);

	} // namespace Command

} // namespace XS
