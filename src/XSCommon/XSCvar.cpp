#include "System/XSInclude.h"

#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"

namespace XS {

	static std::unordered_map<std::string, Cvar*> cvars;
	static bool initialised = false;

	void Cvar::LoadConfig( void ) {
		// ...

		initialised = true;
	}

	void Cvar::WriteConfig( void ) {
		for ( auto itr=cvars.begin(); itr != cvars.end(); ++itr ) {
			const char *name = itr->first.c_str();
			Cvar *cv = itr->second;
			if ( (cv->flags & ARCHIVE) && cv->value.str != cv->value.defaultStr ) {
				#ifdef _DEBUG
					Print( "Saving Cvar '%s' with value '%s'\n", name, cv->value.str.c_str() );
				#endif
			}
		}
	}

	void Cvar::Clean( void ) {
		Print( "Cleaning up cvars\n" );
		for ( auto it=cvars.begin(); it != cvars.end(); ++it ) {
			delete it->second;
		}
		cvars.clear();
	}

	Cvar *Cvar::Create( std::string name, std::string value, uint32_t flags ) {
		Cvar *cvar = cvars[name];
		if ( initialised )
			flags &= ~INIT;

		// check for existing
		if ( cvar ) {
			if ( !value.empty() ) {
				// INIT cvars should not be initialised with differing values
				if ( cvar->flags & INIT )
					Print( String::Format( "WARNING: INIT Cvar '%s' was created twice with values '%s' and '%s'\n", name, cvar->value.str, value ) );
				else
					cvar->Set( value );
			}

			if ( flags != NONE )
				cvar->SetFlags( flags );
			return cvar;
		}

		// allocate and add to map
		cvar = new Cvar( value );
		cvar->value.defaultStr = value;
		cvars[name] = cvar;

		return cvar;
	}

	void Cvar::SetFlags( uint32_t flags ) {
		if ( initialised )
			flags &= ~INIT;

		this->flags = flags;
	}

	bool Cvar::Set( const char *value ) {
		if ( this->flags & READONLY )
			return false;

		this->value.str = value;
		this->value.number = (float)atof( value );
		this->value.integer = atoi( value );
		this->value.boolean = !!this->value.integer;
		this->value.tokens = String::Split( value, ' ' );

		return true;
	}

	bool Cvar::Set( const std::string &value ) {
		return this->Set( value.c_str() );
	}

	bool Cvar::Set( const int value ) {
		return this->Set( String::Format( "%i", value ) );
	}

	bool Cvar::Set( const float value ) {
		return this->Set( String::Format( "%f", value ) );
	}

	bool Cvar::Set( const bool value ) {
		return this->Set( String::Format( "%i", value ) ); // good enough
	}

}; // namespace XS
