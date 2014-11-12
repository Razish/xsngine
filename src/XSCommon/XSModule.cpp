#include <SDL2/SDL_loadso.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSModule.h"
#include "XSCommon/XSString.h"

namespace XS {

	typedef const char * (*initFunc_t)(uint32_t);

	Module::Module( const char *name, uint32_t version ) {
		char relativePath[XS_MAX_FILENAME];
		String::FormatBuffer( relativePath, sizeof(relativePath), "%s" ARCH_STRING DLL_EXT, name );

		char absolutePath[XS_MAX_FILENAME];
		File::GetFullPath( relativePath, absolutePath, sizeof( absolutePath ) );

		handle = SDL_LoadObject( absolutePath );
		if ( !handle ) {
			throw( XSError( String::Format( "Could not find module (%s)", name ).c_str() ) );
			return;
		}

		if ( initFunc_t init = (initFunc_t)SDL_LoadFunction( absolutePath, "LoadModule" ) ) {
			const char *msg = init( version );
			if ( msg ) {
				throw( XSError( msg ) );
			}
		}
		else {
			throw( XSError( String::Format( "Could not load module (%s)", relativePath ).c_str() ) );
		}
	}

	Module::~Module() {
		if ( handle ) {
			SDL_UnloadObject( handle );
			handle = nullptr;
		}
	}

} // namespace XS
