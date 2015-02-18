#pragma once

#include "XSCommon/XSMessageBuffer.h"
namespace XS {

	enum class PrintLevel {
		Normal,
		Debug,
		Developer
	};

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
			PrintLevel level,
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
		inline Indent( int32_t newLevel )
		: level( newLevel )
		{
			console.Indent( level );
		}

		// revert the indent level
		inline ~Indent() {
			console.Indent( -level );
		}
	};

} // namespace XS
