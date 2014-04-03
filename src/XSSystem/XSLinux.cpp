#include "XSSystem/XSInclude.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSGlobals.h"

#include <sys/stat.h>

namespace XS {

	namespace OS {

		void GetCurrentWorkingDirectory( char *cwd, size_t bufferLen ) {
			if ( getcwd( cwd, (int)bufferLen - 1 ) == NULL )
				cwd[0] = '\0';
			//TODO: determine cause of failure via errno
			cwd[FILENAME_MAX - 1] = '\0';
		}

		bool MkDir( const char *path ) {
			if ( mkdir( path, S_IRWXU | S_IXGRP | S_IRGRP ) ) {
				if ( errno != EEXIST )
					return false;
			}

			return true;
		}

		bool ResolvePath( char *outPath, const char *inPath, size_t pathLen ) {
			assert( outPath && inPath );
			if ( !realpath( inPath, outPath ) ) {
				if ( Common::com_developer->GetBool() )
					Console::Print( "Could not resolve path: \"%s\" (errno: %i)\n", inPath, errno );
				outPath[0] = '\0';
			}
		}

		bool Stat( const char *path ) {
			struct stat buf;

			if ( stat( path, &buf ) )
				return false;

			return true;
		}

	} // namespace OS

} // namespace XS
