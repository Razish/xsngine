#include "XSSystem/XSInclude.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"

namespace XS {

	static Cvar *com_path = NULL;

	static const char *modes[FM_NUM_MODES] = {
		"rb", // FM_READ
		"rb", // FM_READ_BINARY
		"wb", // FM_WRITE
		"wb", // FM_WRITE_BINARY
		"a" // FM_APPEND
	};

	void File::Init( void ) {
		com_path = Cvar::Create( "com_path", "", CVAR_INIT );
	}

	void File::GetPath( const char *gamePath, char *outPath, size_t outLen ) {
		String::FormatBuffer( outPath, outLen, "%s/%s", com_path->GetCString( ), gamePath );
	}

	File::File( const char *gamePath, fileMode_t mode ) {
		GetPath( gamePath, path, sizeof( path ) );

		//TODO: create folders if non-existent. requires platform-specific code

		this->mode = mode;
		file = fopen( path, modes[mode] );

		if ( !file ) {
			length = 0L;
			open = false;
			return;
		}
		open = true;
		length = 0L;

		// only read/append modes care about the file length
		if ( mode != FM_WRITE && mode != FM_WRITE_BINARY ) {
			fseek( file, 0L, SEEK_END );
				length = ftell( file );
			fseek( file, 0L, SEEK_SET );

			// account for null terminator
			if ( mode == FM_READ )
				length++;
		}
	}

	void File::Read( byte *buf, size_t len ) const {
		if ( len == 0 )
			len = length;

		size_t res = fread( (void *)buf, 1, len, file );
		if ( res == 0u )
			buf[0] = '\0';
		//TODO: determine cause of failure via feof, ferror

		// account for null terminator
		if ( mode == FM_READ )
			buf[len-1] = '\0';
	}

	void File::AppendString( const char *str ) const {
		fputs( str, file );
	}

	File::~File() {
		if ( file ) {
			fclose( file );
			file = NULL;
		}
	}

} // namespace XS
