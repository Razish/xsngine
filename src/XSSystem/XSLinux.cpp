#include "XSSystem/XSInclude.h"

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

		bool Stat( const char *path ) {
			struct stat buf;

			if ( stat( path, &buf ) )
				return false;

			return true;
		}

	} // namespace OS

} // namespace XS
