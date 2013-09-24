#pragma once

#include "XSCommon/XSCvar.h"

namespace XS {

	namespace Renderer {

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
