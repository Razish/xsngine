#pragma once

namespace XS {

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
		Cvar();
		static bool initialised;

		std::string name;

		// store the value in these formats
		std::string defaultStr;
		std::string fullString;
		struct CvarValue {
			std::string str;
			int integer;
			float number;
			bool boolean;
		};
		std::vector<CvarValue> values; // separated by whitespace

		uint32_t flags;

		// has been modified after creation (by user or code)
		bool modified;

	public:
		static void LoadConfig( void );
		static void WriteConfig( void );
		static void Clean( void );

		// xtors
		Cvar( const std::string &name, const std::string &value = "" );
		static Cvar *Create( std::string name, std::string value="", uint32_t flags=NONE );
		static Cvar *Get( const std::string &name );

		// get/set
		void SetFlags( uint32_t flags );
		const uint32_t Flags( void ) { return flags; }

		bool Set( const std::string &value, bool initial = false );
		bool Set( const char *value, bool initial = false );
		bool Set( const int value, bool initial = false );
		bool Set( const float value, bool initial = false );
		bool Set( const bool value, bool initial = false );

		inline const std::string &GetString( size_t index = 0 ) { return this->values[index].str; }
		inline const char *GetCString( size_t index = 0 ) { return this->values[index].str.c_str(); }
		inline const int GetInt( size_t index = 0 ) { return this->values[index].integer; }
		inline const float GetFloat( size_t index = 0 ) { return this->values[index].number; }
		inline bool GetBool( size_t index = 0 ) { return this->values[index].boolean; }
	};

}; // namespace XS
