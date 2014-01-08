#pragma once

namespace XS {

	namespace Console {

		void	Init		( void );
		void	Print		( const std::string &fmt, ... );
		void	Toggle		( const commandContext_t *context );
		void	Display		( void );
		void	Indent		( int level );

	} // namespace Console
	
	// Instantiate a local (stack-level) Indent object to indent subsequent console prints
	class Indent {
	private:
		Indent(){}
		unsigned int level;
	public:
		Indent( unsigned int level ) { this->level = level; Console::Indent( this->level ); }
		~Indent() { Console::Indent( -level ); }
	};

} // namespace XS
