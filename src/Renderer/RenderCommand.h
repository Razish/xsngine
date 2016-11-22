#pragma once

#include "Renderer/Renderer.h"
#include "Common/Matrix.h"

namespace Renderer {

	struct Material;
	class Model;
	class ShaderProgram;

	extern ShaderProgram *quadProgram;

	//FIXME: polymorphism?
	enum class CommandType {
		DrawQuad,
		DrawModel,
		DrawParticles,
		Screenshot
	};

	struct DrawQuadCommand {

		vector2 pos;
		vector2 size;
		vector2 st1, st2;
		vector4 colour;
		const Material	*material;

	};

	struct DrawModelCommand {

		const Model			*model;
		RenderInfo			 info;

	};

	struct DrawParticlesCommand {

		const Backend::Buffer	*vbo;
		const Backend::Buffer	*ibo;
		const Material			*material;
		size_t					 count;

	};

	struct ScreenshotCommand {

		int32_t		 width;
		int32_t		 height;
		GLuint		 pbo;
		GLsync		 sync;
		const char	*name;

	};

	struct RenderCommand {

	private:
		CommandType type;

	public:
		union {
			DrawQuadCommand			drawQuad;
			DrawModelCommand		drawModel;
			DrawParticlesCommand	drawParticles;
			ScreenshotCommand		screenshot;
		};

		// don't allow default instantiation
		RenderCommand() = delete;

		RenderCommand( CommandType commandType )
		: type( commandType )
		{
		}

		// initialise render command queue
		// create default material, allocate GPU buffers
		static void Init(
			void
		);

		// clean up GPU resources
		static void Shutdown(
			void
		);

		// dispatch the render command to the associated function
		void Execute(
			void
		) const;

	};

	// ???
	void DrawQuad(
		const DrawQuadCommand &cmd
	);

} // namespace Renderer
