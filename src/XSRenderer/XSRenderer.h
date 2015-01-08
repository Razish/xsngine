#pragma once

#include <GL/glew.h>

namespace XS {

	// front-end renderer access

	class Cvar;
	class vector4;

	namespace Renderer {

		extern Cvar *r_clear;
		extern Cvar *r_debug;
		extern Cvar *r_multisample;
		extern Cvar *r_skipRender;
		extern Cvar *r_swapInterval;
		extern Cvar *vid_height;
		extern Cvar *vid_noBorder;
		extern Cvar *vid_width;

		struct Material;
		struct View;
		class Model;

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

		// when called with a null material, will use default quad material
		// when called with a null colour, will use white
		void	DrawQuad	( float x, float y, float w, float h, float s1, float t1, float s2, float t2,
			const vector4 *colour, const Material *material );
		void	DrawModel	( const Model *model );

	} // namespace Renderer

} // namespace XS
