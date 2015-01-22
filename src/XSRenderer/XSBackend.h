#pragma once

namespace XS {

	class Cvar;

	namespace Renderer {

		namespace Backend {

			extern Cvar *r_fov;
			extern Cvar *r_zRange;

			extern GLuint defaultVao;
			extern GLuint defaultPbo;

			void Init(
				void
			);

			void Shutdown(
				void
			);

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
