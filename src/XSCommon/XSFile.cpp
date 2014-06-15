#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSSystem/XSOS.h"

namespace XS {

	static char basePath[XS_MAX_FILENAME];
	static Cvar *com_path;
	static bool initialised = false;

	static const char *modes[FileMode::NUM_MODES] = {
		"rb", // FM_READ
		"rb", // FM_READ_BINARY
		"wb", // FM_WRITE
		"wb", // FM_WRITE_BINARY
		"a" // FM_APPEND
	};

	// returns true if mode is for writing
	static inline bool IsWriteMode( FileMode mode ) {
		switch ( mode ) {
		default:
		case FileMode::READ:
		case FileMode::READ_BINARY:
			return false;
		case FileMode::WRITE:
		case FileMode::WRITE_BINARY:
		case FileMode::APPEND:
			return true;
		}
	}

	void File::Init( void ) {
		// set com_path to current working directory, can be overridden later
		char cwd[FILENAME_MAX];
		OS::GetCurrentWorkingDirectory( cwd, sizeof(cwd) );
		com_path = Cvar::Create( "com_path", cwd, "Base directory to load assets from", CVAR_INIT );
	}

	// finalise the basepath
	void File::SetBasePath( void ) {
		OS::ResolvePath( basePath, com_path->GetCString(), sizeof(basePath) );
		Console::Print( "Base path: %s\n", basePath );

		initialised = true;
	}

	// returns true if requested file does not attempt directory traversal
	static bool ValidPath( const char *gamePath ) {
		if ( strstr( gamePath, ".." ) || strstr( gamePath, "::" ) ) {
			return false;
		}

		return true;
	}

	// normalise path separators for the current platform
	void File::ReplaceSeparators( char *path ) {
		for ( char *s = path; *s; s++ ) {
			if ( *s == '/' || *s == '\\' )
				*s = PATH_SEP;
		}
	}

	// return the full OS path for the specified relative gamepath
	//	e.g. "C:\xsngine\textures\black.jpg" for "textures/black.jpg"
	bool File::GetFullPath( const char *gamePath, char *outPath, size_t outLen ) {
		if ( !initialised ) {
			outPath[0] = '\0';
			return false;
		}

		if ( !ValidPath( gamePath ) ) {
			Console::Print( "WARNING: attempted directory traversal: \"%s\"\n", gamePath );
			outPath[0] = '\0';
			return false;
		}

		String::FormatBuffer( outPath, outLen, "%s%c%s", basePath, PATH_SEP, gamePath );
		File::ReplaceSeparators( outPath );

		return true;
	}

	// create the folders necessary to store the specified file
	static bool CreatePath( const char *fullPath ) {
		char path[XS_MAX_FILENAME];
		String::Copy( path, fullPath, sizeof(path) );
		for ( char *s = strchr( path, PATH_SEP ) + 1; s && *s; s++ ) {
			if ( *s == PATH_SEP ) {
				*s = '\0';
				if ( !OS::MkDir( path ) ) {
					throw( XSError( String::Format( "failed to create path \"%s\"", path ).c_str() ) );
				}
				*s = PATH_SEP;
			}
		}
		return true;
	}

	// opens a file ready for read/write
	// upon failure, file.open will be false and file.length will be 0
	File::File( const char *gamePath, FileMode mode ) {
		path[0] = '\0';

		if ( !GetFullPath( gamePath, path, sizeof( path ) ) ) {
			Clear();
			return;
		}

		// create folders required to store file if non-existent
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
			if ( mode == READ ) {
				length++;
			}
		}
	}

	// read len bytes into buf
	// if len is 0, read as much as possible
	void File::Read( byte *buf, size_t len ) const {
		if ( len == 0u ) {
			len = length;
		}

		size_t res = fread( (void *)buf, 1, len, file );
		if ( res == 0u ) {
			buf[0] = '\0';
		}
		//TODO: determine cause of failure via feof, ferror

		// account for null terminator
		if ( mode == READ ) {
			buf[len-1] = '\0';
		}
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
