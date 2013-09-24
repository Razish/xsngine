#include "XSSystem/XSInclude.h"

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
			if ( (cv->flags & ARCHIVE) && cv->modified && cv->value.str != cv->value.defaultStr ) {
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

	// private
	Cvar::Cvar() {
		this->flags = NONE;
		this->modified = false;
	}

	// public
	Cvar::Cvar( std::string &value ) {
		this->flags = NONE;
		this->modified = false;
		Set( value );
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
					Print( "WARNING: INIT Cvar '%s' was created twice with values '%s' and '%s'\n", name.c_str(), cvar->value.str.c_str(), value.c_str() );

				// don't initialise a cvar if it already exists/has been set
				else if ( !cvar->modified )
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

	Cvar *Cvar::Get( const std::string &name ) {
		Cvar *cv = cvars[name];
		
		if ( cv )
			return cv;

		cv = new Cvar();
		cvars[name] = cv;
	
		return cv;
	}

	void Cvar::SetFlags( uint32_t flags ) {
		if ( initialised )
			flags &= ~INIT;

		this->flags = flags;
	}

	bool Cvar::Set( const char *value ) {
		if ( this->flags & READONLY )
			return false;

		this->value.tokens = String::Split( value, ' ' );
		this->value.str = this->value.tokens[0];
		this->value.number = (float)atof( this->value.str.c_str() );
		this->value.integer = atoi( this->value.str.c_str() );
		this->value.boolean = !!this->value.integer;
		
		this->modified = true;

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
