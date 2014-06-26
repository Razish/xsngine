#include <Windows.h> // curse you, monolithic headers!
#include <sys/stat.h>
#include <direct.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSGlobals.h"
#include "XSSystem/XSOS.h"

namespace XS {

	namespace OS {

		void GetCurrentWorkingDirectory( char *cwd, size_t bufferLen ) {
			_getcwd( cwd, (int)bufferLen - 1 );
			cwd[FILENAME_MAX - 1] = '\0';
		}

		bool MkDir( const char *path ) {
			if ( !CreateDirectory( path, nullptr ) ) {
				if ( GetLastError() != ERROR_ALREADY_EXISTS )
					return false;
			}

			return true;
		}

		bool ResolvePath( char *outPath, const char *inPath, size_t pathLen ) {
			SDL_assert( outPath && inPath && "OS::ResolvePath called with invalid parameters" );

			if ( /*Stat( inPath ) &&*/ !_fullpath( outPath, inPath, pathLen ) ) {
				if ( Common::com_developer->GetBool() ) {
					console.Print( "Could not resolve path: \"%s\" (%s)\n", inPath, strerror( errno ) );
				}
				outPath[0] = '\0';

				return false;
			}

			return true;
		}

		bool Stat( const char *path ) {
			struct _stat buf;

			if ( _stat( path, &buf ) ) {
				return false;
			}

			return true;
		}

	} // namespace OS

} // namespace XS
