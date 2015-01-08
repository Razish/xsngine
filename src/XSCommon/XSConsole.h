#pragma once

namespace XS {

	class MessageBuffer;

	extern struct Console {
		unsigned int indentation;
		MessageBuffer *buffer;

		Console();
		~Console();

		inline void Indent( int level ) {
			indentation += level;
		}

		void Print( const char *fmt, ... );
		void DebugPrint( const char *fmt, ... );

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
