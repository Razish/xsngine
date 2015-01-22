#pragma once

#include "XSCommon/XSMessageBuffer.h"
namespace XS {

	extern struct Console {
		int32_t			indentation;
		MessageBuffer	*buffer;

		Console();
		inline ~Console() {
			delete buffer;
		}

		// modify the indentation level for future prints
		// called by an Indent object
		inline void Indent( int32_t level ) {
			indentation += level;
		}

		// print a line to the console buffer
		void Print(
			const char *fmt,
			...
		);

		// print a line to the console buffer if running a debug build
		void DebugPrint(
			const char *fmt,
			...
		);

	private:
		// used internally
		// append a line to the list, accounting for split lines
		void Append(
			const char *text,
			bool multiLine
		);
	} console;

	// Instantiate a local (stack-level) Indent object to indent subsequent console prints
	class Indent {
	private:
		int32_t		level;

	public:
		// don't allow default instantiation
		Indent() = delete;
		Indent( const Indent& ) = delete;
		Indent& operator=( const Indent& ) = delete;

		// set the indent level
		inline Indent( int32_t level )
		: level( level )
		{
			console.Indent( level );
		}

		// revert the indent level
		inline ~Indent() {
			console.Indent( -level );
		}
	};

} // namespace XS
