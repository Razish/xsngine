#pragma once

#include <GL/glew.h>

#include "XSCommon/XSCvar.h"

namespace XS {

	namespace Renderer {

		extern Cvar *r_debug;
		extern Cvar *r_multisample;
		extern Cvar *r_skipRender;
		extern Cvar *r_swapInterval;
		extern Cvar *vid_height;
		extern Cvar *vid_noBorder;
		extern Cvar *vid_width;

		struct Material;
		struct View;

		// private
		void	RegisterCvars	( void );
		void	CreateDisplay	( void );
		void	DestroyDisplay	( void );

		// public
		void	Init	( void );
		void	Shutdown( void );
		void	Update	( void );

		// access
		void	RegisterView( View *view );
		void	SetView		( View *view );

		// render commands
		void	DrawQuad( float x, float y, float w, float h, float s1, float t1, float s2, float t2,
			const Material& material);

	} // namespace Renderer

} // namespace XS
