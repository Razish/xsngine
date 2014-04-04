#include "XSSystem/XSInclude.h"
#include "XSSystem/XSPlatform.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSGlobals.h"
#include "XSSystem/XSOS.h"

namespace XS {

	static const char *modes[FM_NUM_MODES] = {
		"rb", // FM_READ
		"rb", // FM_READ_BINARY
		"wb", // FM_WRITE
		"wb", // FM_WRITE_BINARY
		"a" // FM_APPEND
	};

	// returns true if mode is for writing
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

	// normalise path separators for the current platform
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

	// returns false if fullpath is outside of basepath
	// if paths are relative, they will be resolved
	static bool CompareBaseDirectory( const char *basepath, const char *fullpath ) {
		char b1[FILENAME_MAX];
		const char *s1 = basepath;
		if ( strstr( s1, ".." ) || strstr( s1, "::" ) ) {
			OS::ResolvePath( b1, s1, sizeof(b1) );
			s1 = b1;
		}

		char b2[FILENAME_MAX];
		const char *s2 = fullpath;
		if ( strstr( s2, ".." ) || strstr( s2, "::" ) ) {
			OS::ResolvePath( b2, b2, sizeof(b2) );
			s2 = b2;
		}

		if ( !String::Compare( s1, s2, strlen( s1 ) ) )
			return true;

		return false;
	}

	// create the folders necessary to store the specified file
	static bool CreatePath( const char *fullpath ) {
		char path[FILENAME_MAX], basepath[FILENAME_MAX];

		OS::ResolvePath( basepath, Common::com_path->GetCString(), sizeof(basepath) );
		if ( !CompareBaseDirectory( basepath, fullpath ) ) {
			Console::Print( "WARNING: attempted directory traversal: \"%s\"\n", fullpath );
			return false;
		}

		String::Copy( path, fullpath, sizeof(path) );
		File::ReplaceSeparators( path );

		for ( char *s = strchr( path, PATH_SEP ) + 1; s && *s; s++ ) {
			if ( *s == PATH_SEP ) {
				*s = '\0';
				if ( !OS::MkDir( path ) )
					throw( XSError( String::Format( "failed to create path \"%s\"", path ).c_str() ) );
				*s = PATH_SEP;
			}
		}
		return true;
	}

	void File::Init( void ) {
		Common::com_path = Cvar::Create( "com_path", "", CVAR_INIT );
	}

	// return the full OS path for the specified relative gamepath
	//	e.g. "C:\xsngine\textures\black.jpg" for "textures/black.jpg"
	void File::GetPath( const char *gamePath, char *outPath, size_t outLen ) {
		char fullpath[FILENAME_MAX];
		String::FormatBuffer( fullpath, sizeof(fullpath), "%s%c%s", Common::com_path->GetCString(), PATH_SEP, gamePath );
		File::ReplaceSeparators( fullpath );
		OS::ResolvePath( outPath, fullpath, outLen );
	}

	// opens a file ready for read/write
	// upon failure, file.open will be false and file.length will be 0
	File::File( const char *gamePath, fileMode_t mode ) {
		path[0] = '\0';
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

	// read len bytes into buf
	// if len is 0, read as much as possible
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
