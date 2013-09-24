#pragma once

#include "XSCommon/XSCvar.h"

namespace XS {

	namespace Renderer {

		extern Cvar *r_fov,
			*r_multisample,
			*r_swapInterval,
			*vid_height,
			*vid_noBorder,
			*vid_width;

		// private
		void CheckGLErrors( const char *filename, int line );
		void RegisterCvars( void );
		void CreateDisplay( void );
		void DestroyDisplay( void );
		void InitGL( void );
		void SetViewport( const int width, const int height );

		// public
		void Init( void );
		void Shutdown( void );
		void Update( void );

	};

}; // namespace XS
