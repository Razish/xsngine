#pragma once

namespace XS {

	namespace Console {

		void	Init		( void );
		void	Print		( std::string fmt, ... );
		void	Toggle		( const commandContext_t *context );
		void	Display		( void );
		void	Indent		( int level );

	} // namespace Console
	
	class Indent {
	private:
		Indent(){}
		int level;
	public:
		Indent( int level ) { this->level = level; Console::Indent( this->level ); }
		~Indent() { Console::Indent( -level ); }
	};

} // namespace XS
