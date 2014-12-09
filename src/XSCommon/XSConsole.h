#pragma once

#include "XSCommon/XSMessageBuffer.h"

namespace XS {

	extern struct Console {
		unsigned int indentation;
		MessageBuffer *buffer;

		Console()
		: indentation( 0u )
		{
			buffer = new MessageBuffer( "console.log" );
		}
		~Console() {
			delete buffer;
		}

		void Print( const char *fmt, ... );

		void Indent( int level ) {
			indentation += level;
		}

	private:
		void Append( const char *text, bool multiLine );
	} console;

	// Instantiate a local (stack-level) Indent object to indent subsequent console prints
	class Indent {
	private:
		unsigned int level;

	public:
		// don't allow default instantiation
		Indent() = delete;
		Indent( const Indent& ) = delete;
		Indent& operator=( const Indent& ) = delete;

		Indent( int level )
		: level( (unsigned)level )
		{
			console.Indent( level );
		}

		~Indent() {
			console.Indent( -(signed)level );
		}
	};

} // namespace XS
