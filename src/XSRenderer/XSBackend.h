#pragma once

namespace XS {

	namespace Renderer {

		namespace Backend {

			void	Init		( void );
			void	Shutdown	( void );

			extern Cvar *r_fov;
			extern Cvar *r_zRange;

			extern GLuint defaultVao;
			extern GLuint defaultPbo;

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
