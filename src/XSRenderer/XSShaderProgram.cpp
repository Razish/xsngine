#include "XSSystem/XSInclude.h"

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

		#define MAX_SHADER_LENGTH (1024*32)

		static std::vector<ShaderProgram*> programs;
		static std::vector<Shader*> shaders;

		static const ShaderProgram *lastProgramUsed = NULL;

		static bool initialised = false;

		void GLSL_Init( void ) {
			if ( !SDL_GL_ExtensionSupported( "GL_ARB_shader_objects" ) )
			{
				Print( "...GLSL extension NOT loaded.\n"
						"Required OpenGL extensions not available.\n" );
				return;
			}

			Print( "GLSL extension loaded\n" );

			initialised = true;
		}

		void GLSL_Cleanup( void ) {
			Print( "Cleaning up GLSL shaders\n" );

			for ( auto it = programs.begin(); it != programs.end(); ++it )
				delete *it;
			programs.clear();

			for ( auto it = shaders.begin(); it != shaders.end(); ++it ) 
				delete *it;
			shaders.clear();

			lastProgramUsed = NULL;

			initialised = false;
		}

		static void GLSL_OutputInfoLog( int objectID ) {
			int logLength = 0;
			char *logText = NULL;
			
			Indent indent(1);

			glGetObjectParameterivARB( objectID, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logLength );

			if ( logLength > 0 ) {
				logText = new char[logLength];
				glGetInfoLogARB( objectID, logLength, NULL, logText );

				Print( String::Format( "%s\n", logText ) );

				delete[] logText;
			}
		}

		//
		// Shaders
		//

		static std::string LoadSource( const char *path ) {
			std::ifstream file( path, std::ios::in );
			
			if ( !file.is_open() ) {
				Print( String::Format( "LoadSource: failed to open %s for reading\n", path ) );
				return "";
			}

			std::string line;
			std::stringstream source;
			while ( std::getline( file, line ) )
				source << line << std::endl;
			file.close();

			return source.str();
		}

		void Shader::Create( const char *path, const char *source, int shaderType ) {
			char *shaderCode = NULL;
			int statusCode = 0;

			if ( !initialised )
				throw( "ShaderProgram(): GLSL extension not initialised" );

			shaderCode = new char[MAX_SHADER_LENGTH];
			String::Copy( shaderCode, source, MAX_SHADER_LENGTH );

			id = glCreateShaderObjectARB( shaderType );
			String::Copy( name, path, sizeof( name ) );
			type = (shaderType == GL_VERTEX_SHADER_ARB) ? VertexShader : FragmentShader;

			if ( !id ) {
				delete[] shaderCode;
				r_glsl->Set( false );

				throw( String::Format( "Shader(): Failed to create shader object for shader '%s'.\n", path ) );
			}

			glShaderSourceARB( id, 1, (const GLcharARB **)&shaderCode, NULL );
			if ( glGetError() == GL_INVALID_OPERATION ) {
				GLSL_OutputInfoLog( id );

				glDeleteObjectARB( id );
				delete[] shaderCode;
				r_glsl->Set( false );

				throw( String::Format( "Shader(): Invalid source code in shader '%s'\n", path ) );
			}

			delete[] shaderCode;

			glCompileShaderARB( id );
			glGetObjectParameterivARB( id, GL_OBJECT_COMPILE_STATUS_ARB, &statusCode );

			if ( statusCode == GL_FALSE ) {
				GLSL_OutputInfoLog( id );

				glDeleteObjectARB( id );
				r_glsl->Set( false );

				throw( String::Format( "Shader(): Failed to compile shader source for shader '%s'\n", path ) );
			}

			GLSL_OutputInfoLog( id );

			shaders.push_back( this );
		}

		Shader::Shader( ShaderType type, const char *name ) {
			switch( type ) {
			case FragmentShader:
				Create( name, LoadSource( String::Format( "shaders/f_%s.glsl", name ).c_str() ).c_str(), GL_FRAGMENT_SHADER_ARB );
				break;

			case VertexShader:
				Create( name, LoadSource( String::Format( "shaders/v_%s.glsl", name ).c_str() ).c_str(), GL_VERTEX_SHADER_ARB );
				break;

			default:
				throw( String::Format( "Shader(): Unknown type %d", type ) );
			}
		}

		Shader::~Shader() {
			glDeleteObjectARB( id );
		}

		//
		// Shader Programs
		//

		ShaderProgram::ShaderProgram() {
			if ( !initialised )
				throw( "ShaderProgram(): GLSL extension not initialised" );

			id = glCreateProgramObjectARB();
			if ( !id )
				Print( String::Format( "ShaderProgram(): Failed to create program with internal ID %d\n", programs.size() ) );

			fragmentShader = vertexShader = NULL;
			uniforms = attributes = NULL;

			programs.push_back( this );
		}

		// save some typing..
		ShaderProgram::ShaderProgram( const char *vertexShaderName, const char *fragmentShaderName ) {
			if ( !initialised )
				throw( "ShaderProgram(): GLSL extension not initialised" );

			id = glCreateProgramObjectARB();
			if ( !id )
				Print( String::Format( "ShaderProgram(): Failed to create program with internal ID %d\n", programs.size() ) );

			fragmentShader = vertexShader = NULL;
			uniforms = attributes = NULL;

			if ( vertexShaderName )
				AttachShader( new Shader( VertexShader, vertexShaderName ) );
			if ( fragmentShaderName )
				AttachShader( new Shader( FragmentShader, fragmentShaderName ) );

			Link();
			Bind();

			programs.push_back( this );
		}

		ShaderProgram::~ShaderProgram() {
			if ( fragmentShader )
				glDetachObjectARB( id, fragmentShader->id );
			if ( vertexShader )
				glDetachObjectARB( id, vertexShader->id );

			glDeleteObjectARB( id );

			if ( uniforms )
				delete uniforms;
			if ( attributes )
				delete attributes;
		}

		ProgramVariable *ShaderProgram::GetUniformLocation( const char *name ) {
			ProgramVariable *variable = uniforms, *prev = NULL;

			while ( variable && strcmp( variable->name, name ) ) {
				prev = variable;
				variable = variable->next;
			}

			if ( !variable ) {
				if ( !prev ) {
					uniforms = new ProgramVariable;
					variable = uniforms;
				}
				else {
					prev->next = new ProgramVariable;
					variable = prev->next;
				}

				variable->name = name;
				variable->next = NULL;
				variable->location = glGetUniformLocationARB( id, name );
			}

			return variable;
		}

		void ShaderProgram::AttachShader( Shader *shader ) {
			if ( !shader )
				return;

			switch ( shader->type ) {
			case FragmentShader:
				if ( fragmentShader != shader ) {
					fragmentShader = shader;
					glAttachObjectARB( id, shader->id );
				}
				break;

			case VertexShader:
				if ( vertexShader != shader ) {
					vertexShader = shader;
					glAttachObjectARB( id, shader->id );
				}
				break;

			default:
				throw( String::Format( "AttachShader(): Unknown type %d", shader->type ) );
			}
		}

		void ShaderProgram::Link( void ) {
			int statusCode = 0;

			glLinkProgramARB( id );
			glGetObjectParameterivARB( id, GL_OBJECT_LINK_STATUS_ARB, &statusCode );

			if ( statusCode == GL_FALSE ) {
				Print( String::Format( "Failed to link program %d\n", id ) );
				GLSL_OutputInfoLog( id );
			}
		}

		void ShaderProgram::Bind( void ) {
			if ( lastProgramUsed != this ) {
				glUseProgramObjectARB( id );
				lastProgramUsed = this;
			}
		}

		void ShaderProgram::SetUniform1( const char *name, int i ) {
			glUniform1iARB( GetUniformLocation( name )->location, i );
		}

		void ShaderProgram::SetUniform1( const char *name, float f ) {
			glUniform1fARB( GetUniformLocation( name )->location, f );
		}

		void ShaderProgram::SetUniform2( const char *name, float f1, float f2 ) {
			glUniform2fARB( GetUniformLocation( name )->location, f1, f2 );
		}

		void ShaderProgram::SetUniform3( const char *name, float f1, float f2, float f3 ) {
			glUniform3fARB( GetUniformLocation( name )->location, f1, f2, f3 );
		}

		void ShaderProgram::SetUniform4( const char *name, float f1, float f2, float f3, float f4 ) {
			glUniform4fARB( GetUniformLocation( name )->location, f1, f2, f3, f4 );
		}

	} // namespace Renderer

} // namespace XS
