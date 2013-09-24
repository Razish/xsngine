#include "XSSystem/XSInclude.h"

#include "XSCommon/XSCvar.h"
#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"

namespace XS {

	Cvar *File::com_path;
	static const char *modes[File::NUM_MODES] = {
		"rb+",
		"w",
		"a"
	};

	void File::Init( void ) {
		com_path = Cvar::Get( "com_path" );
	}

	void File::GetPath( const char *gamePath, char *outPath, size_t outLen ) {
		String::sprintf( outPath, outLen, "%s/%s", com_path->GetCString(), gamePath );
	}

	File::File( const char *gamePath, Mode mode ) {
		GetPath( gamePath, path, sizeof( path ) );

		this->mode = mode;
		file = fopen( path, modes[mode] );

		if ( !file ) {
			length = 0;
			return;
		}

		length = ftell( file );
	}

	void File::Read( byte *buf, size_t len ) {
		if ( len == 0 )
			len = length;
		fread( (void *)buf, 1, len, file );
	}

	void File::AppendString( const char *str ) {
		if ( mode != APPEND ) {
			Console::Print( "Tried to append to file not opened with APPEND mode: %s", path );
			return;
		}

		fwrite( str, 1, strlen(str), file );
	}

	File::~File() {
		fclose( file );
	}

} // namespace XS
