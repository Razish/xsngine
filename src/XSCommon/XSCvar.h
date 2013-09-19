#pragma once

namespace XS {

	extern std::unordered_map<std::string, class Cvar*> cvars;

	class Cvar {
	public:
		enum Flags {
			NONE		= 0X0000,	// temporary storage
			ARCHIVE		= 0x0001,	// will be saved to disk
			INIT		= 0X0002,	// can only be set during initialisation (i.e. not runtime)
			LATCH		= 0X0004,	// value will not be updated until relevant subsystem is restarted
			READONLY	= 0X0010,	// can only be set by code
			DEVELOPER	= 0x0020,	// can only be set in developer mode
		};

	private:
		// store the value in these formats
		struct {
			std::string str, defaultStr;
			std::vector<std::string> tokens; // separated by whitespace
			int integer;
			float number;
			bool boolean;
		} value;

		uint32_t flags;

		// has been modified after creation (by user or code)
		bool modified;

	public:
		// static funcs
		static void LoadConfig( void );
		static void WriteConfig( void );
		static void LockInit( void ) { /*initialised = true;*/ }

		// xtors
		Cvar( std::string &value ) { this->Set( value ); }
		static Cvar *Create( std::string name, std::string value, uint32_t flags );

		// get/set
		void SetFlags( uint32_t flags );
		const uint32_t Flags( void ) { return flags; }

		bool Set( const std::string &value );
		bool Set( const char *value );
		bool Set( const int value );
		bool Set( const float value );

		inline const std::string &String( void ) { return this->value.str; }
		inline const int Int( void ) { return this->value.integer; }
		inline const float Float( void ) { return this->value.number; }
		inline const bool Bool( void ) { return this->value.boolean; }
		inline const std::string &Token( int idx ) { return this->value.tokens[idx]; }
	};

}; // namespace XS
