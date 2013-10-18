#include "XSSystem/XSInclude.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSFile.h"

namespace XS {

	#define DEFAULT_CONFIG "cfg/xsn.cfg"

	static std::unordered_map<std::string, Cvar*> cvars;
	bool Cvar::initialised = false;

	void Cvar::LoadConfig( void ) {
		File f( DEFAULT_CONFIG, FM_READ );

		if ( f.length ) {
			char *buffer = new char[f.length];
				f.Read( (byte *)buffer );
				char *current = strtok( buffer, "\n" );
				while ( current ) {
					Command::Append( current );
					Command::ExecuteBuffer();
					current = strtok( NULL, "\n" );
				}
			delete[] buffer;
		}

		initialised = true;
	}

	void Cvar::WriteConfig( void ) {
		File f( DEFAULT_CONFIG, FM_WRITE );

		if ( !f.open ) {
			Console::Print( "Failed to load default config! (" DEFAULT_CONFIG ")\n" );
			return;
		}

		for ( auto itr=cvars.begin(); itr != cvars.end(); ++itr ) {
			const char *name = itr->first.c_str();
			Cvar *cv = itr->second;
			if ( (cv->flags & CVAR_ARCHIVE) && cv->modified && cv->fullString != cv->defaultStr )
				f.AppendString( String::Format( "set %s %s\n", name, cv->fullString.c_str() ).c_str() );
		}
	}

	void Cvar::Clean( void ) {
		Console::Print( "Cleaning up cvars\n" );
		for ( auto it=cvars.begin(); it != cvars.end(); ++it )
			delete it->second;
		cvars.clear();
	}

	// private
	Cvar::Cvar() {
		cvars[name] = this;

		flags = CVAR_NONE;
		modified = false;
	}

	// public
	Cvar::Cvar( const std::string &name, const std::string &value, uint32_t flags ) {
		cvars[name] = this;

		this->name = name;
		this->flags = flags;
		modified = false;
		Set( value, true );
	}

	Cvar *Cvar::Create( std::string name, std::string value, uint32_t flags ) {
		Cvar *cvar = cvars[name];
		if ( initialised )
			flags &= ~CVAR_INIT;

		// check for existing
		if ( cvar ) {
			if ( !value.empty() ) {
				// INIT cvars should not be initialised with differing values
				if ( cvar->flags & CVAR_INIT )
					Console::Print( "WARNING: CVAR_INIT Cvar '%s' was created twice with values '%s' and '%s'\n",
						name.c_str(), cvar->fullString.c_str(), value.c_str() );

				// don't initialise a cvar if it already exists/has been set
				else if ( !cvar->modified )
					cvar->Set( value, true );
			}

			if ( flags != CVAR_NONE )
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

	void Cvar::List( void ) {
		for ( auto itr=cvars.begin(); itr != cvars.end(); ++itr )
			Console::Print( "%-32s : %s\n", itr->first.c_str(), itr->second->fullString.c_str() );
	}

	void Cvar::SetFlags( uint32_t flags ) {
		if ( initialised )
			flags &= ~CVAR_INIT;

		this->flags = flags;
	}

	bool Cvar::Set( const char *value, bool initial ) {
		if ( flags & CVAR_READONLY ) {
			Console::Print( "Attempt to set read-only cvar '%s'\n", name.c_str() );
			return false;
		}

		fullString = value;

		values.clear();
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
