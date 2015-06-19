#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)

#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSCvar.h"
#include "XSSystem/XSOS.h"

namespace XS {

	namespace OS {

		void GetCurrentWorkingDirectory( char *cwd, size_t bufferLen ) {
			if ( getcwd( cwd, (int)bufferLen - 1 ) == nullptr ) {
				cwd[0] = '\0';
			}
			//TODO: determine cause of failure via errno
			cwd[FILENAME_MAX - 1] = '\0';
		}

		bool GetFileTime( const char *path, int32_t *outTime ) {
			struct stat buf;

			if ( stat( path, &buf ) ) {
				*outTime = 0;
				return false;
			}

			long int modifiedTime = buf.st_mtime;
			*outTime = static_cast<int32_t>( modifiedTime );

			return true;
		}

		uint32_t GetPathMax( void ) {
			long result = pathconf( "/", _PC_PATH_MAX );
			if ( result < 0 ) {
				result = std::min( _POSIX_PATH_MAX, PATH_MAX );
			}
			return result;
		}

		bool MkDir( const char *path ) {
			if ( mkdir( path, S_IRWXU | S_IXGRP | S_IRGRP ) ) {
				if ( errno != EEXIST ) {
					return false;
				}
			}

			return true;
		}

		bool ResolvePath( char *outPath, const char *inPath, size_t pathLen ) {
			SDL_assert( outPath && inPath && "OS::ResolvePath called with invalid parameters" );

			if ( !Stat( inPath ) || !realpath( inPath, outPath ) ) {
				console.Print( PrintLevel::Developer, "Could not resolve path: \"%s\" (%s)\n",
					inPath,
					strerror( errno )
				);
				outPath[0] = '\0';

				return false;
			}

			return true;
		}

		bool RmDir( const char *path ) {
			rmdir( path );
			return true;
		}

		bool RmFile( const char *path ) {
			remove( path );
			return true;
		}

		bool Stat( const char *path ) {
			struct stat buf;

			if ( stat( path, &buf ) ) {
				return false;
			}

			return true;
		}

	} // namespace OS

} // namespace XS

#endif
