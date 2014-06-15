#include <map>
#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"


namespace XS {

	static std::unordered_map<std::string, Cvar*> cvars;
	bool Cvar::initialised = false;

	void Cvar::WriteCvars( std::string &str ) {
		std::map<std::string, Cvar*> sorted( cvars.begin(), cvars.end() );

		for ( const auto &it : sorted ) {
			const char *name = it.first.c_str();
			Cvar *cv = it.second;
			if ( !cv ) {
				// shouldn't happen
				if ( Common::com_developer->GetBool() ) {
					Console::Print( "WriteCvars: NULL cvar \"%s\"\n", name );
				}
				continue;
			}
			if ( (cv->flags & CVAR_ARCHIVE) && cv->modified && cv->fullString != cv->defaultStr ) {
				str += String::Format( "set %s \"%s\"\n", name, cv->fullString.c_str() );
			}
		}
	}

	void Cvar::Clean( void ) {
		Console::Print( "Cleaning up cvars\n" );
		for ( const auto &it : cvars ) {
			delete it.second;
		}
		cvars.clear();
	}

	// private
	Cvar::Cvar() {
		cvars[name] = this;

		SetFlags( CVAR_NONE );
		modified = false;
	}

	// public
	Cvar::Cvar( const std::string &name, const std::string &value, const std::string &description, uint32_t flags )
		: name( name ), defaultStr( value ), description( description ), modified( false ) {

		cvars[name] = this;
		SetFlags( flags );
		Set( value, true );
	}

	Cvar *Cvar::Create( std::string name, std::string value, std::string description, uint32_t flags ) {
		Cvar *cvar = cvars[name];
		if ( initialised ) {
			flags &= ~CVAR_INIT;
		}

		// check for existing
		if ( cvar ) {
			if ( !value.empty() ) {
				// INIT cvars should not be initialised with differing values
				if ( cvar->flags & CVAR_INIT ) {
					Console::Print( "WARNING: CVAR_INIT Cvar \"%s\" was created twice with values \"%s\" and \"%s\"\n",
						name.c_str(), cvar->fullString.c_str(), value.c_str() );
				}

				// don't initialise a cvar if it already exists/has been set
				else if ( !cvar->modified ) {
					cvar->Set( value, true );
				}

				cvar->description = description;
			}

			if ( flags != CVAR_NONE ) {
				cvar->SetFlags( flags );
			}

			return cvar;
		}

		// allocate and add to map
		return new Cvar( name, value, description, flags );
	}

	Cvar *Cvar::Get( const std::string &name ) {
		Cvar *cv = cvars[name];

		if ( cv ) {
			return cv;
		}

		return NULL; //new Cvar( name );
	}

	void Cvar::List( void ) {
		Console::Print( "Listing cvars...\n" );

		std::map<std::string, Cvar*> sorted( cvars.begin(), cvars.end() );

		Indent indent( 1 );
		for ( const auto &cvar : sorted ) {
			char buf[64];
			String::FormatBuffer( buf, sizeof(buf), "%s \"%s\"", cvar.first.c_str(), cvar.second->fullString.c_str() );
			Console::Print( "%-48s: %s\n", buf, cvar.second->description.c_str() );
		}
	}

	void Cvar::SetFlags( uint32_t flags ) {
		if ( initialised ) {
			flags &= ~CVAR_INIT;
		}

		this->flags = flags;
	}

	bool Cvar::Set( const char *value, bool initial ) {
		if ( !initial && (flags & CVAR_READONLY) ) {
			Console::Print( "Attempt to set read-only cvar \"%s\"\n", name.c_str() );
			return false;
		}

		fullString = value;

		values.clear();
		const char delim = ' ';
		std::vector<std::string> tokens = String::Split( value, delim );
		for ( const auto &it : tokens ) {
			CvarValue newValue;

			newValue.str = it;
			const char *cstr = newValue.str.c_str();
			newValue.real = atof( cstr );
			newValue.number = (float)newValue.real;
			newValue.integer = atoi( cstr );
			newValue.boolean = !!newValue.integer;

			values.push_back( newValue );
		}

		if ( !initial ) {
			modified = true;
		}

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
		return Set( String::Format( "%i", !!value ), initial ); // good enough
	}

} // namespace XS
