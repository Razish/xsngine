#pragma once

namespace XS {

	namespace Renderer {

		// private
		void	CheckGLErrors	( const char *filename, int line );
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
		void	DrawQuad( float x, float y, float w, float h, float s1, float t1, float s2, float t2, const Texture *texture );

	} // namespace Renderer

} // namespace XS
