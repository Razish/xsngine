#include <unordered_map>
#include <map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"


namespace XS {

	static std::unordered_map<std::string, Cvar *> cvars;
	bool Cvar::initialised = false;

	void Cvar::WriteCvars( std::string &str ) {
		std::map<std::string, Cvar *> sorted( cvars.begin(), cvars.end() );

		for ( const auto &it : sorted ) {
			const char *name = it.first.c_str();
			const Cvar *cv = it.second;
			if ( !cv ) {
				// shouldn't happen
				if ( Common::com_developer->GetBool() ) {
					console.Print( "%s NULL cvar \"%s\"\n", XS_FUNCTION, name );
				}
				continue;
			}
			if ( (cv->flags & CVAR_ARCHIVE) && cv->modified && cv->fullString != cv->defaultStr ) {
				str += String::Format( "set %s \"%s\"\n", name, cv->fullString.c_str() );
			}
		}
	}

	void Cvar::Clean( void ) {
		console.DebugPrint( "Cleaning up cvars\n" );
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

	// private
	Cvar::Cvar( const std::string &name, const std::string &value, const std::string &description, uint32_t flags )
	: name( name ), defaultStr( value ), description( description ), modified( false )
	{
		cvars[name] = this;
		SetFlags( flags );
		Set( value, true );
	}

	// public
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
					console.Print( "WARNING: CVAR_INIT Cvar \"%s\" was created twice with values \"%s\" and \"%s\"\n",
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

		return nullptr;
	}

	void Cvar::List( void ) {
		console.Print( "Listing cvars...\n" );

		std::map<std::string, Cvar *> sorted( cvars.begin(), cvars.end() );
		std::vector<std::string> keyValues;

		Indent indent( 1 );
		size_t maxLen = 1u;
		for ( const auto &cvar : sorted ) {
			keyValues.push_back( String::Format( "%s = \"%s\"", cvar.first.c_str(), cvar.second->fullString.c_str() ) );
			const size_t len = strlen( keyValues.back().c_str() );
			if ( len > maxLen ) {
				maxLen = len;
			}
		}
		uint32_t i = 0;
		for ( const auto &cvar : sorted ) {
			console.Print( "%-*s: %s\n", maxLen + 1, keyValues[i++].c_str(), cvar.second->description.c_str() );
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
			console.Print( "Attempt to set read-only cvar \"%s\"\n", name.c_str() );
			return false;
		}

		fullString = value;

		values.clear();
		const char delim = ' ';
		std::vector<std::string> tokens = String::Split( value, delim );
		for ( const auto &it : tokens ) {
			CvarValue newValue = {};

			newValue.str = it;
			const char *cstr = newValue.str.c_str();
			newValue.real = atof( cstr );
			newValue.number = static_cast<float>( newValue.real );
			newValue.integer = atoi( cstr );
			newValue.boolean = !!newValue.integer; // coerce to boolean

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

	bool Cvar::Set( const int32_t value, bool initial ) {
		return Set( String::Format( "%i", value ), initial );
	}

	bool Cvar::Set( const float value, bool initial ) {
		return Set( String::Format( "%f", value ), initial );
	}

	bool Cvar::Set( const bool value, bool initial ) {
		return Set( String::Format( "%i", !!value ), initial ); // good enough
	}

} // namespace XS
