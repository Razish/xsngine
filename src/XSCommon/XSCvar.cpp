#include <unordered_map>
#include <map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"


namespace XS {

	static std::unordered_map<std::string, Cvar *> cvars;
	bool Cvar::initialised = false; // flag for init-only cvars

	// public, static
	void Cvar::WriteCvars( std::string &str ) {
		std::map<std::string, Cvar *> sorted( cvars.begin(), cvars.end() );

		for ( const auto &it : sorted ) {
			const char *name = it.first.c_str();
			const Cvar *cv = it.second;
			if ( !cv ) {
				// shouldn't happen
				if ( Common::com_developer->GetBool() ) {
					console.Print( PrintLevel::Normal, "%s NULL cvar \"%s\"\n", XS_FUNCTION, name );
				}
				continue;
			}
			if ( (cv->flags & CVAR_ARCHIVE) && cv->modified && cv->fullString != cv->defaultStr ) {
				str += String::Format( "set %s \"%s\"\n", name, cv->fullString.c_str() );
			}
		}
	}

	// public, static
	void Cvar::Clean( void ) {
		console.Print( PrintLevel::Debug, "Cleaning up cvars\n" );
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
	Cvar::Cvar( const std::string &cvarName, const std::string &value, const std::string &cvarDescription,
		uint32_t cvarFlags )
	: name( cvarName ), defaultStr( value ), description( cvarDescription ), flags( 0u ), modified( false )
	{
		cvars[name] = this;
		SetFlags( cvarFlags );
		Set( value, true );
	}

	// public, static
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
					console.Print( PrintLevel::Normal,
						"WARNING: CVAR_INIT Cvar \"%s\" was created twice with values \"%s\" and \"%s\"\n",
						name.c_str(),
						cvar->fullString.c_str(),
						value.c_str()
					);
				}

				// not init-only, only (re)set if it hasn't been modified
				else if ( !cvar->modified ) {
					// this will happen if the cvar has been set via config or cmdline, and then we try to create it
					//	 which means we don't want to override the requested value with the initial value
					cvar->Set( value, true );
				}

				cvar->description = description;
				cvar->defaultStr = value;
			}

			if ( flags != CVAR_NONE ) {
				cvar->SetFlags( flags );
			}

			return cvar;
		}

		// new cvar, allocate and add to map
		// this will only happen the first time it is requested:
		//	first config load for archived cvars
		//	set via cmdline
		//	default value when registering cvars
		return new Cvar( name, value, description, flags );
	}

	// public, static
	Cvar *Cvar::Get( const std::string &name ) {
		for ( auto &cv : cvars ) {
			if ( !String::CompareCase( cv.first.c_str(), name.c_str() ) ) {
				return cv.second;
			}
		}
		return nullptr;
	}

	// public, static
	void Cvar::List( void ) {
		console.Print( PrintLevel::Normal, "Listing cvars...\n" );

		std::map<std::string, Cvar *> sorted( cvars.begin(), cvars.end() );
		std::vector<std::string> keyValues;
		keyValues.reserve( sorted.size() );

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
			console.Print( PrintLevel::Normal, "%-*s: %s\n",
				maxLen + 1,
				keyValues[i++].c_str(),
				cvar.second->description.c_str()
			);
		}
	}

	// public
	void Cvar::SetFlags( uint32_t newFlags ) {
		if ( initialised ) {
			newFlags &= ~CVAR_INIT;
		}

		flags = newFlags;
	}

	// public
	bool Cvar::Set( const char *value, bool initial ) {
		// if the cvar already has a value and is read-only, don't allow modifying it
		if ( !initial && (flags & CVAR_READONLY) ) {
			console.Print( PrintLevel::Normal, "Attempt to set read-only cvar \"%s\"\n", name.c_str() );
			return false;
		}

		if ( !initial ) {
			if ( !initialised && modified ) {
				// loaded from config, but has been set before (i.e. via cmdline)
				// in this case, we don't want to override the requested cmdline value
				return false;
			}
		}

		// first, store the desired value as a string
		fullString = value;

		// then tokenise it
		values.clear();
		const char delim = ' ';
		std::vector<std::string> tokens;
		String::Split( value, delim, tokens );

		for ( const auto &it : tokens ) {
			// now store each token in multiple formats
			CvarValue newValue = {};

			newValue.str = it;
			const char *cstr = newValue.str.c_str();
			newValue.real64 = atof( cstr );
			newValue.real32 = static_cast<real32_t>( newValue.real64 );
			newValue.int32 = atoi( cstr );
			newValue.uint32 = strtoul( cstr, nullptr, 0 );
			newValue.boolean = !!newValue.uint32; // coerce to boolean

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

	bool Cvar::Set( const uint32_t value, bool initial ) {
		return Set( String::Format( "%u", value ), initial );
	}

	bool Cvar::Set( const real32_t value, bool initial ) {
		return Set( String::Format( "%f", value ), initial );
	}

	bool Cvar::Set( const bool value, bool initial ) {
		return Set( String::Format( "%i", !!value ), initial ); // good enough
	}

} // namespace XS
