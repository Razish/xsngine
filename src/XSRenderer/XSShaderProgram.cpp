#include "System/XSInclude.h"

#include "GLee/GLee.h"
#include "SDL2/SDL.h"

#include "XSCommon/XSCvar.h"
#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Renderer {

		#define MAX_GLSL_PROGRAMS (24)
		#define MAX_GLSL_SHADERS (MAX_GLSL_PROGRAMS * 2)
		#define MAX_SHADER_LENGTH (32768)

		static unsigned int numPrograms = 0;
		static glslProgram_t glslPrograms[MAX_GLSL_PROGRAMS];

		static unsigned int numShaders = 0;
		static glslShader_t glslShaders[MAX_GLSL_SHADERS];

		static const glslProgram_t *lastProgramUsed = NULL;

		static bool initialised = false;

		void GLSL_Init( void ) {
			if ( !SDL_GL_ExtensionSupported( "GL_ARB_shader_objects" ) )
			{
				Print( "...GLSL extension NOT loaded.\n"
						"Required OpenGL extensions not available.\n" );
				return;
			}

			memset( glslPrograms, 0, sizeof( glslPrograms ) );
			memset( glslShaders, 0, sizeof( glslShaders ) );

			Print( "GLSL extension loaded\n" );

			initialised = true;
		}

		static void GLSL_CleanupVariable( glslProgramVariable_t *variable ) {
			if ( variable && variable->next )
				GLSL_CleanupVariable( variable->next );
			else
				free( (void *)variable );
		}

		void GLSL_Cleanup( void ) {
			glslProgram_t *program = NULL;
			glslShader_t *shader = NULL;
			unsigned int i=0;

			Print( "Cleaning up GLSL shaders\n" );

			for ( i=0; i<numPrograms; i++ ) {
				program = &glslPrograms[i];
				if ( program->id == 0 )
					continue;

				if ( program->fragmentShader )
					glDetachObjectARB( program->id, program->fragmentShader->id );

				if ( program->vertexShader )
					glDetachObjectARB( program->id, program->vertexShader->id );

				glDeleteObjectARB( program->id );

				GLSL_CleanupVariable( program->uniforms );
				GLSL_CleanupVariable( program->attributes );
			}

			for ( i=0; i<numShaders; i++ ) {
				shader = &glslShaders[i];

			//	if ( shader->id == 0 )
			//		continue;

				glDeleteObjectARB( shader->id );
			}

			lastProgramUsed = NULL;
			numPrograms = numShaders = 0;
			memset( glslPrograms, 0, sizeof( glslPrograms ) );
			memset( glslShaders, 0, sizeof( glslShaders ) );

			initialised = false;
		}

		static void GLSL_OutputInfoLog( int objectID ) {
			int logLength = 0;
			char *logText = NULL;
			
			Indent indent(1);

			glGetObjectParameterivARB( objectID, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logLength );

			if ( logLength > 0 ) {
				logText = (char *)malloc( logLength );
				glGetInfoLogARB( objectID, logLength, NULL, logText );

				Print( String::Format( "%s\n", logText ) );

				free( (void *)logText );
			}
		}

		static glslProgramVariable_t *GLSL_GetUniformLocation( glslProgram_t *program, const char *name ) {
			glslProgramVariable_t *variable = program->uniforms, *prev = NULL;

			while ( variable && strcmp( variable->name, name ) ) {
				prev = variable;
				variable = variable->next;
			}

			if ( !variable ) {
				if ( !prev ) {
					program->uniforms = (glslProgramVariable_t *)malloc( sizeof( glslProgramVariable_t ) );
					variable = program->uniforms;
				}
				else {
					prev->next = (glslProgramVariable_t *)malloc( sizeof( glslProgramVariable_t ) );
					variable = prev->next;
				}

				variable->name = name;
				variable->next = NULL;
				variable->location = glGetUniformLocationARB( program->id, name );
			}

			return variable;
		}

#if 0
		static glslProgramVariable_t *GLSL_GetAttributeLocation( glslProgram_t *program, const char *name ) {
			glslProgramVariable_t *variable = program->attributes;

			while ( variable && strcmp( variable->name, name ) )
				variable = variable->next;

			if ( !variable ) {
				variable = (glslProgramVariable_t *)malloc( sizeof( glslProgramVariable_t ) );

				variable->name = name;
				variable->next = NULL;
				variable->location = glGetAttribLocationARB( program->id, name );
			}

			return variable;
		}
#endif

		glslProgram_t *GLSL_CreateProgram( void ) {
			glslProgram_t *program = NULL;

			if ( !initialised )
				throw( "GLSL_CreateProgram: GLSL extension not initialised" );

			if ( numPrograms >= MAX_GLSL_PROGRAMS )
				throw( "GLSL_CreateProgram: Maximum number of GLSL programs exceeded" );

			program = &glslPrograms[numPrograms];

			program->id = glCreateProgramObjectARB();
			if ( !program->id )
				Print( String::Format( "GLSL_CreateProgram: Failed to create program with internal ID %d\n", numPrograms ) );

			program->fragmentShader = NULL;
			program->vertexShader = NULL;

			numPrograms++;

			return program;
		}

		static glslShader_t *GLSL_CreateShader( const char *path, const char *source, int shaderType ) {
			glslShader_t *shader = NULL;
			char *shaderCode = NULL;
			int statusCode = 0;

			if ( numShaders >= MAX_GLSL_SHADERS ) {
				Print( "GLSL_CreateShader: Maximum number of GLSL shaders exceeded.\n" );
				return NULL;
			}

			shaderCode = (char *)malloc( MAX_SHADER_LENGTH );
			if ( !shaderCode ) {
				Print( String::Format( "GLSL_CreateShader: Unable to allocate memory for shader code for shader file '%s'.\n", path ) );
				return NULL;
			}
			String::Copy( shaderCode, source, MAX_SHADER_LENGTH );

			shader = &glslShaders[numShaders];
			shader->id = glCreateShaderObjectARB( shaderType );
			String::Copy( shader->name, path, sizeof( shader->name ) );
			shader->type = (shaderType == GL_VERTEX_SHADER_ARB) ? VERTEX_SHADER : FRAGMENT_SHADER;

			if ( !shader->id ) {
				Print( String::Format( "GLSL_CreateShader: Failed to create shader object for shader '%s'.\n", path ) );

				free( (void *)shaderCode );
				r_glsl->Set( false );

				return NULL;
			}

			glShaderSourceARB( shader->id, 1, (const GLcharARB **)&shaderCode, NULL );
			if ( glGetError() == GL_INVALID_OPERATION ) {
				Print( String::Format( "GLSL_CreateShader: Invalid source code in shader '%s'\n", path ) );

				GLSL_OutputInfoLog( shader->id );

				glDeleteObjectARB( shader->id );
				free( (void *)shaderCode );
				r_glsl->Set( false );

				return NULL;
			}

			free( (void *)shaderCode );

			glCompileShaderARB( shader->id );
			glGetObjectParameterivARB( shader->id, GL_OBJECT_COMPILE_STATUS_ARB, &statusCode );

			if ( statusCode == GL_FALSE ) {
				Print( String::Format( "GLSL_CreateShader: Failed to compile shader source for shader '%s'\n", path ) );
				GLSL_OutputInfoLog( shader->id );

				glDeleteObjectARB( shader->id );
				r_glsl->Set( false );

				return NULL;
			}

			GLSL_OutputInfoLog( shader->id );

			numShaders++;

			return shader;
		}

		// save some typing..
		glslProgram_t *GLSL_CreateProgram( const char *vertexShader, const char *fragmentShader ) {
			glslProgram_t *program = GLSL_CreateProgram();
			
			if ( vertexShader )
				GLSL_AttachShader( program, GLSL_CreateVertexShader( vertexShader ) );
			if ( fragmentShader )
				GLSL_AttachShader( program, GLSL_CreateFragmentShader( fragmentShader ) );

			GLSL_LinkProgram( program );
			GLSL_UseProgram( program );

			return program;
		}

		static std::string GLSL_LoadSource( const char *path ) {
			std::ifstream file( path, std::ios::in );
			
			if ( !file.is_open() ) {
				Print( String::Format( "GLSL_LoadSource: failed to open %s for reading\n", path ) );
				return "";
			}

			std::string line;
			std::stringstream source;
			while ( std::getline( file, line ) )
				source << line << std::endl;
			file.close();

			return source.str();
		}

		glslShader_t *GLSL_CreateFragmentShader( const char *name ) {
			return GLSL_CreateShader( name, GLSL_LoadSource( String::Format( "shaders/f_%s.glsl", name ).c_str() ).c_str(), GL_FRAGMENT_SHADER_ARB );
		}

		glslShader_t *GLSL_CreateVertexShader( const char *name ) {
			return GLSL_CreateShader( name, GLSL_LoadSource( String::Format( "shaders/v_%s.glsl", name ).c_str() ).c_str(), GL_VERTEX_SHADER_ARB );
		}

		void GLSL_AttachShader( glslProgram_t *program, glslShader_t *shader ) {
			if ( shader == NULL )
				return;

			switch ( shader->type ) {
			case VERTEX_SHADER:
				if ( program->vertexShader != shader ) {
					program->vertexShader = shader;
					glAttachObjectARB( program->id, shader->id );
				}
				break;

			case FRAGMENT_SHADER:
				if ( program->fragmentShader != shader ) {
					program->fragmentShader = shader;
					glAttachObjectARB( program->id, shader->id );
				}
				break;
			}
		}

		void GLSL_LinkProgram( const glslProgram_t *program ) {
			int statusCode = 0;

			glLinkProgramARB( program->id );
			glGetObjectParameterivARB( program->id, GL_OBJECT_LINK_STATUS_ARB, &statusCode );

			if ( statusCode == GL_FALSE ) {
				Print( String::Format( "Failed to link program %d\n", program->id ) );
				GLSL_OutputInfoLog( program->id );
			}
		}

		void GLSL_UseProgram( const glslProgram_t *program ) {
			if ( lastProgramUsed != program ) {
				int id = program ? program->id : 0;

				glUseProgramObjectARB( id );
				lastProgramUsed = program;
			}
		}

		void GLSL_SetUniform1i( glslProgram_t *program, const char *name, int i ) {
			glslProgramVariable_t *variable = GLSL_GetUniformLocation( program, name );
			glUniform1iARB( variable->location, i );
		}

		void GLSL_SetUniform1f( glslProgram_t *program, const char *name, float f ) {
			glslProgramVariable_t *variable = GLSL_GetUniformLocation( program, name );
			glUniform1fARB( variable->location, f );
		}

		void GLSL_SetUniform2f( glslProgram_t *program, const char *name, float f1, float f2 ) {
			glslProgramVariable_t *variable = GLSL_GetUniformLocation( program, name );
			glUniform2fARB( variable->location, f1, f2 );
		}

		void GLSL_SetUniform3f( glslProgram_t *program, const char *name, float f1, float f2, float f3 ) {
			glslProgramVariable_t *variable = GLSL_GetUniformLocation( program, name );
			glUniform3fARB( variable->location, f1, f2, f3 );
		}

		void GLSL_SetUniform4f( glslProgram_t *program, const char *name, float f1, float f2, float f3, float f4 ) {
			glslProgramVariable_t *variable = GLSL_GetUniformLocation( program, name );
			glUniform4fARB( variable->location, f1, f2, f3, f4 );
		}

	} // namespace Renderer

} // namespace XS
