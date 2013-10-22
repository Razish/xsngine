#pragma once

namespace XS {

	// uint32_t
	#define CVAR_NONE		(0x0000u)	// temporary storage
	#define CVAR_ARCHIVE	(0x0001u)	// will be saved to disk
	#define CVAR_INIT		(0x0002u)	// can only be set during initialisation (i.e. not runtime)
	#define CVAR_LATCH		(0x0004u)	// value will not be updated until relevant subsystem is restarted
	#define CVAR_READONLY	(0x0008u)	// can only be set by code
	#define CVAR_DEVELOPER	(0x0010u)	// can only be set in developer mode

	class Cvar {
	private:
		Cvar();
		Cvar( const std::string &name, const std::string &value = "", uint32_t flags = CVAR_NONE );

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
		static void WriteCvarsToFile( const File &f );
		static void Clean( void );

		static bool initialised;

		static Cvar *Create( std::string name, std::string value="", uint32_t flags=CVAR_NONE );
		static Cvar *Get( const std::string &name );
		static void List( void );

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

} // namespace XS
