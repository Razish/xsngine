#pragma once

#include <string>
#include <vector>

namespace XS {

	#define CVAR_NONE		(0x0000u)	// temporary storage
	#define CVAR_ARCHIVE	(0x0001u)	// will be saved to disk
	#define CVAR_INIT		(0x0002u)	// can only be set during initialisation (i.e. not runtime)
	#define CVAR_LATCH		(0x0004u)	// value will not be updated until relevant subsystem is restarted
	#define CVAR_READONLY	(0x0008u)	// can only be set by code

	struct CvarValue {
		std::string 	str = "";
		int32_t			int32 = 0;
		uint32_t		uint32 = 0u;
		real32_t		real32 = 0.0f;
		real64_t		real64 = 0.0;
		bool			boolean = false;
	};

	class Cvar {
	private:
		// used internally
		// construct a new cvar object and add it to the global list
		// called by Cvar::Create()
		Cvar(
			const std::string &cvarName,
			const std::string &value = "",
			const std::string &cvarDescription = "none",
			uint32_t cvarFlags = CVAR_NONE
		);
		Cvar();

		std::string				name = "";
		std::string				defaultStr = "";
		std::string				description = ""; // short description of the cvar
		std::string				fullString = "";
		std::vector<CvarValue>	values; // separated by whitespace
		uint32_t				flags = 0u; // archive, read-only, etc
		bool					modified = false; // has been modified after creation (by user or code)

	public:
		// indicates whether the game has been initialised for cvars that may only be set on the command line
		static bool		initialised;

		// don't allow copy constructor or assignment operator, must be created uniquely
		Cvar( const Cvar& ) = delete;
		Cvar& operator=( const Cvar& ) = delete;

		// write cvars to the end of the string
		static void WriteCvars(
			std::string &str
		);

		// remove all cvars
		static void Clean(
			void
		);

		// register a new cvar in the global list
		static Cvar *Create(
			std::string name,
			std::string value = "",
			std::string description = "none",
			uint32_t flags = CVAR_NONE
		) XS_WARN_UNUSED_RESULT;

		// fetch a handle to an existing cvar
		// returns nullptr if none are found
		static Cvar *Get(
			const std::string &name
		) XS_WARN_UNUSED_RESULT;

		// print all cvars, flags and values to console
		static void List(
			void
		);

		// set the cvar's default str
		void SetDefaultString(
			const char *defaultStr
		);

		// modify flags for a cvar
		// can not unset INIT flag if game is already initialised
		void SetFlags(
			uint32_t newFlags
		);

		// set a new value for the cvar
		bool Set(
			const std::string &value,
			bool initial = false
		);
		bool Set(
			const char *value,
			bool initial = false
		);
		bool Set(
			const int32_t value,
			bool initial = false
		);
		bool Set(
			const uint32_t value,
			bool initial = false
		);
		bool Set(
			const real32_t value,
			bool initial = false
		);
		bool Set(
			const bool value,
			bool initial = false
		);

		// retrieve a value for the cvar at the given index (e.g. index 1 for "640 480" will be "480"
		inline const std::string &GetDefaultString( void ) const XS_WARN_UNUSED_RESULT {
			return defaultStr;
		}
		inline const std::string &GetFullString( void ) const XS_WARN_UNUSED_RESULT {
			return fullString;
		}
		inline const char *GetFullCString( void ) const XS_WARN_UNUSED_RESULT {
			return fullString.c_str();
		}
		inline const std::string &GetString( size_t index = 0u ) const XS_WARN_UNUSED_RESULT {
			return values[index].str;
		}
		inline const char *GetCString( size_t index = 0u ) const XS_WARN_UNUSED_RESULT {
			return values[index].str.c_str();
		}
		inline int32_t GetInt32( size_t index = 0u ) const XS_WARN_UNUSED_RESULT {
			return values[index].int32;
		}
		inline uint32_t GetUInt32( size_t index = 0u ) const XS_WARN_UNUSED_RESULT {
			return values[index].uint32;
		}
		inline real32_t GetReal32( size_t index = 0u ) const XS_WARN_UNUSED_RESULT {
			return values[index].real32;
		}
		inline real64_t GetReal64( size_t index = 0u ) const XS_WARN_UNUSED_RESULT {
			return values[index].real64;
		}
		inline bool GetBool( size_t index = 0u ) const XS_WARN_UNUSED_RESULT {
			return values[index].boolean;
		}
	};

} // namespace XS
