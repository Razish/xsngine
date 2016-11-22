#include <SDL2/SDL_loadso.h>

#include "Common/Common.h"
#include "Common/Error.h"
#include "Common/File.h"
#include "Common/Module.h"
#include "Common/String.h"

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

	if ( InitFunc init = (InitFunc)SDL_LoadFunction( absolutePath, "LoadModule" ) ) {
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
