#include "XSSystem/XSInclude.h"
#include "XSSystem/XSPlatform.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSError.h"
#include "XSSystem/XSOS.h"

namespace XS {

	static Cvar *com_path = NULL;

	static const char *modes[FM_NUM_MODES] = {
		"rb", // FM_READ
		"rb", // FM_READ_BINARY
		"wb", // FM_WRITE
		"wb", // FM_WRITE_BINARY
		"a" // FM_APPEND
	};

	static inline bool IsWriteMode( fileMode_t mode ) {
		switch ( mode ) {
		default:
		case FM_READ:
		case FM_READ_BINARY:
			return false;
		case FM_WRITE:
		case FM_WRITE_BINARY:
		case FM_APPEND:
			return true;
		}
	}

	void File::ReplaceSeparators( char *path ) {
		bool skip = false;

		for ( char *s = path; *s; s++ ) {
			if ( *s == '/' || *s == '\\' ) {
				if ( !skip ) {
					*s = PATH_SEP;
					skip = true;
				}
				else
					memmove( s, s + 1, strlen( s ) );
			}
			else
				skip = false;
		}
	}

	bool CreatePath( const char *fullpath ) {
		char path[FILENAME_MAX];

		if ( strstr( fullpath, ".." ) || strstr( fullpath, "::" ) ) {
			Console::Print( "WARNING: refusing to create relative path \"%s\"\n", fullpath );
			return false;
		}

		String::Copy( path, fullpath, sizeof(path) );
		File::ReplaceSeparators( path );

		for ( char *s = strchr( path, PATH_SEP ) + 1; s != NULL && *s; s++ ) {
			if ( *s == PATH_SEP ) {
				*s = '\0';
				if ( !OS::MkDir( path ) )
					throw( XSError( String::Format( "FS_CreatePath: failed to create path \"%s\"", path ) ) );
				*s = PATH_SEP;
			}
		}
		return true;
	}

	void File::Init( void ) {
		com_path = Cvar::Create( "com_path", "", CVAR_INIT );
	}

	void File::GetPath( const char *gamePath, char *outPath, size_t outLen ) {
		String::FormatBuffer( outPath, outLen, "%s/%s", com_path->GetCString( ), gamePath );
		File::ReplaceSeparators( outPath );
	}

	File::File( const char *gamePath, fileMode_t mode ) {
		GetPath( gamePath, path, sizeof( path ) );

		//TODO: create folders if non-existent. requires platform-specific code
		if ( IsWriteMode( mode ) ) {
			if ( !CreatePath( path ) ) {
				Clear();
				return;
			}
		}
		else {
			if ( !OS::Stat( path ) ) {
				Clear();
				return;
			}
		}

		this->mode = mode;
		file = fopen( path, modes[mode] );

		if ( !file ) {
			Clear();
			return;
		}
		open = true;
		length = 0L;

		// only read/append modes care about the file length
		if ( !IsWriteMode( mode ) ) {
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

	void File::Clear( void ) {
		length = 0L;
		open = false;
		file = NULL;
	}

	File::~File() {
		if ( file ) {
			fclose( file );
			file = NULL;
		}
	}

} // namespace XS
