#include "System/XSInclude.h"

#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"

namespace XS {

	std::unordered_map<std::string, Cvar*> cvars;

	void Cvar::LoadConfig( void ) {
		// ...
	}

	void Cvar::WriteConfig( void ) {
		for ( auto itr=cvars.begin(); itr != cvars.end(); ++itr ) {
#ifdef _DEBUG
			Print( "Saving Cvar '%s' with value '%s'\n", itr->first.c_str(), itr->second->String().c_str() );
#endif
		}
	}

	Cvar *Cvar::Create( std::string name, std::string value, uint32_t flags ) {
		Cvar *cvar = cvars[name];
	//	if ( initialised )
	//		flags &= ~INIT;

		// check for existing
		if ( cvar ) {
			cvar->Set( value );
			if ( flags )
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
	//	if ( initialised )
	//		flags &= ~INIT;

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

}; // namespace XS
