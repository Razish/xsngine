#pragma once

#include <vector>
#include <string>

namespace XS {

	typedef std::vector<std::string> CommandContext;
	typedef void (*CommandFunc)( const CommandContext * const context );

	namespace Command {

		// initialise the command system
		void Init(
			void
		);

		// associate a console command with a function
		bool AddCommand(
			const char *name,
			CommandFunc cmd
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
