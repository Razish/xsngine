#include "XSSystem/XSInclude.h"

#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"

namespace XS {

	static std::unordered_map<std::string, Cvar*> cvars;
	bool Cvar::initialised = false;

	void Cvar::LoadConfig( void ) {
		// ...

		initialised = true;
	}

	void Cvar::WriteConfig( void ) {
		for ( auto itr=cvars.begin(); itr != cvars.end(); ++itr ) {
			const char *name = itr->first.c_str();
			Cvar *cv = itr->second;
			if ( (cv->flags & ARCHIVE) && cv->modified && cv->fullString != cv->defaultStr ) {
				#ifdef _DEBUG
					Console::Print( "Saving Cvar '%s' with value '%s'\n", name, cv->fullString.c_str() );
				#endif
			}
		}
	}

	void Cvar::Clean( void ) {
		Console::Print( "Cleaning up cvars\n" );
		for ( auto it=cvars.begin(); it != cvars.end(); ++it ) {
			delete it->second;
		}
		cvars.clear();
	}

	// private
	Cvar::Cvar() {
		cvars[name] = this;

		flags = NONE;
		modified = false;
	}

	// public
	Cvar::Cvar( const std::string &name, const std::string &value ) {
		cvars[name] = this;

		this->name = name;
		flags = NONE;
		modified = false;
		Set( value, true );
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
					Console::Print( "WARNING: INIT Cvar '%s' was created twice with values '%s' and '%s'\n", name.c_str(), cvar->fullString.c_str(), value.c_str() );

				// don't initialise a cvar if it already exists/has been set
				else if ( !cvar->modified )
					cvar->Set( value, true );
			}

			if ( flags != NONE )
				cvar->SetFlags( flags );

			return cvar;
		}

		// allocate and add to map
		cvar = new Cvar( name, value );
		cvar->defaultStr = value;

		return cvar;
	}

	Cvar *Cvar::Get( const std::string &name ) {
		Cvar *cv = cvars[name];
		
		if ( cv )
			return cv;

		return new Cvar( name );
	}

	void Cvar::SetFlags( uint32_t flags ) {
		if ( initialised )
			flags &= ~INIT;

		this->flags = flags;
	}

	bool Cvar::Set( const char *value, bool initial ) {
		if ( flags & READONLY )
			return false;

		fullString = value;

		const char delim = ' ';
		std::vector<std::string> tokens = String::Split( value, delim );
		for ( auto it = tokens.begin(); it != tokens.end(); ++it ) {
			CvarValue newValue;

			newValue.str = *it;
			newValue.number = (float)atof( newValue.str.c_str() );
			newValue.integer = atoi( newValue.str.c_str() );
			newValue.boolean = !!newValue.integer;

			values.push_back( newValue );
		}
		
		if ( !initial )
			modified = true;

		return true;
	}

	bool Cvar::Set( const std::string &value, bool initial ) {
		return Set( value.c_str(), initial );
	}

	bool Cvar::Set( const int value, bool initial ) {
		return Set( String::Format( "%i", value ), initial );
	}

	bool Cvar::Set( const float value, bool initial ) {
		return Set( String::Format( "%f", value ), initial );
	}

	bool Cvar::Set( const bool value, bool initial ) {
		return Set( String::Format( "%i", value ), initial ); // good enough
	}

} // namespace XS
