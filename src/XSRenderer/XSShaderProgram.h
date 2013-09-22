#pragma once

namespace XS {

	namespace Renderer {

		typedef enum glslShaderType_e {
			VERTEX_SHADER=0,
			FRAGMENT_SHADER
		} glslShaderType_t;

		typedef struct glslShader_s {
			int					id;
			char				name[XS_MAX_FILENAME];
			glslShaderType_t	type; // fragment/vertex
		} glslShader_t;

		typedef struct glslProgramVariable_s {
			const char	*name;
			int			location;

			struct glslProgramVariable_s *next;
		} glslProgramVariable_t;

		typedef struct glslProgram_s {
			int						id;

			glslShader_t			*vertexShader, *fragmentShader;
			glslProgramVariable_t	*uniforms, *attributes;
		} glslProgram_t;

		// Function prototypes
		void			GLSL_Init( void );
		void			GLSL_Cleanup( void );

		glslProgram_t *	GLSL_CreateProgram( void );
		glslProgram_t *	GLSL_CreateProgram( const char *vertexShader, const char *fragmentShader );
		glslShader_t *	GLSL_CreateFragmentShader( const char *name );
		glslShader_t *	GLSL_CreateVertexShader( const char *name );
		void			GLSL_AttachShader( glslProgram_t *program, glslShader_t *shader );
		void			GLSL_LinkProgram( const glslProgram_t *program );
		void			GLSL_UseProgram( const glslProgram_t *program );

		void			GLSL_SetUniform1i( glslProgram_t *program, const char *name, int i );
		void			GLSL_SetUniform1f( glslProgram_t *program, const char *name, float f );
		void			GLSL_SetUniform2f( glslProgram_t *program, const char *name, float f1, float f2 );
		void			GLSL_SetUniform3f( glslProgram_t *program, const char *name, float f1, float f2, float f3 );
		void			GLSL_SetUniform4f( glslProgram_t *program, const char *name, float f1, float f2, float f3, float f4 );

	} // namespace Renderer

} // namespace XS
