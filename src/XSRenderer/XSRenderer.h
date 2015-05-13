#pragma once

#include <GL/glew.h>

namespace XS {

	// front-end renderer access

	class Cvar;
	struct vector4;

	namespace Renderer {

		extern Cvar *r_debug;

		namespace Backend {
			class Buffer;
		} // namespace Backend

		struct Material;
		struct View;
		class Model;

		extern struct RendererState {
			bool valid;

			struct Window {
				bool		valid;

				uint32_t	width;
				uint32_t	height;
				real32_t	aspectRatio;
			} window;

			struct Driver {
				const char *vendor;
				const char *renderer;
				const char *coreVersion;
				const char *shaderVersion;
			} driver;
		} state;

		// used internally
		// register cvars owned by the renderer
		void RegisterCvars(
			void
		);

		// used internally
		// create a window with valid render context
		void CreateDisplay(
			void
		);

		// used internally
		// clean up window and render context
		void DestroyDisplay(
			void
		);

		// start up the renderer
		// also creates a window and registers cvars
		void Init(
			void
		);

		// shutdown the renderer
		void Shutdown(
			void
		);

		// render all views and flip the backbuffer
		void Update(
			real64_t dt
		);

		//
		// views
		//

		// register a view to be rendered
		void RegisterView(
			View *view
		);

		// set the current view for subsequent rendering
		void SetCurrentView(
			View *view
		);

		// retrieve the currently bound view
		const View *GetCurrentView(
			void
		);

		//
		// render commands
		//

		// when called with a null material, will use default quad material
		// when called with a null colour, will use white
		void DrawQuad(
			real32_t x,
			real32_t y,
			real32_t w,
			real32_t h,
			real32_t s1,
			real32_t t1,
			real32_t s2,
			real32_t t2,
			const vector4 *colour,
			const Material *material
		);

		// ???
		void DrawModel(
			const Model *model
		);

		// ???
		void DrawParticles(
			const Backend::Buffer *vbo,
			const Backend::Buffer *ibo,
			const Material *material,
			size_t count
		);

	} // namespace Renderer

} // namespace XS
