#pragma once

namespace XS {

	namespace Renderer {

		// private
		void	CheckGLErrors	( const char *filename, int line );
		void	RegisterCvars	( void );
		void	CreateDisplay	( void );
		void	DestroyDisplay	( void );
		void	InitGL			( void );
		void	SetViewport		( const int width, const int height );

		// public
		void	Init			( void );
		void	Shutdown		( void );
		void	Update			( void );

		// access
		void	DrawQuad		( float x, float y, float w, float h, float s1, float t1, float s2, float t2, const Texture *texture );

	} // namespace Renderer

} // namespace XS
