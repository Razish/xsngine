#pragma once

#include <vector>
#include <string>

namespace XS {

	using CommandContext = std::vector<std::string>;
	using CommandFunc = void (*)( const CommandContext &context );

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
