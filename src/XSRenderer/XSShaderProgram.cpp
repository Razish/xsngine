#include "XSSystem/XSInclude.h"

#include <GL/glew.h>
#include "SDL2/SDL.h"

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSError.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Renderer {

		#define MAX_SHADER_LENGTH (1024*32) // 32kb

		const ShaderProgram *ShaderProgram::lastProgramUsed = NULL;

		static const int shaderTypes[Shader::NUM_SHADER_TYPES] = {
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER
		};

		void ShaderProgram::Init( void ) {
			lastProgramUsed = NULL;
		}

		static void OutputProgramInfoLog( int program ) {
			int logLength = 0;
			char *logText = NULL;

			Indent indent(1);

			glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logLength );

			if ( logLength > 1 ) {
				logText = new char[logLength];
				glGetProgramInfoLog( program, logLength, NULL, logText );

				Console::Print( "%s\n", logText );

				delete[] logText;
			}
		}

		static void OutputShaderInfoLog( int shader ) {
			int logLength = 0;
			char *logText = NULL;

			Indent indent (1);

			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength );

			if ( logLength > 1 ) {
				logText = new char[logLength];
				glGetShaderInfoLog( shader, logLength, NULL, logText );

				Console::Print( "%s\n", logText );

				delete[] logText;
			}
		}

		//
		// Shaders
		//

		void Shader::Create( const char *path, const char *source, int shaderType ) {
			char *shaderCode = NULL;
			int statusCode = 0;

			shaderCode = new char[MAX_SHADER_LENGTH];
			String::Copy( shaderCode, source, MAX_SHADER_LENGTH );

			id = glCreateShader( shaderType );
			String::Copy( name, path, sizeof( name ) );
			type = (shaderType == GL_VERTEX_SHADER) ? VERTEX : FRAGMENT;

			if ( !id ) {
				delete[] shaderCode;

				throw( XSError( String::Format( "Shader(): Failed to create shader object for shader \"%s\"\n", path ).c_str() ) );
			}

			glShaderSource( id, 1, (const GLchar **)&shaderCode, NULL );
			if ( glGetError() == GL_INVALID_OPERATION ) {
				OutputShaderInfoLog( id );

				glDeleteShader( id );
				delete[] shaderCode;

				throw( XSError( String::Format( "Shader(): Invalid source code in shader \"%s\"\n", path ).c_str() ) );
			}

			delete[] shaderCode;

			glCompileShader( id );
			glGetShaderiv( id, GL_COMPILE_STATUS, &statusCode );

			if ( statusCode == GL_FALSE ) {
				OutputShaderInfoLog (id);

				glDeleteShader( id );

				throw( XSError( String::Format( "Shader(): Failed to compile shader source for shader \"%s\"\n", path ).c_str() ) );
			}

			OutputShaderInfoLog (id);
		}

		Shader::Shader( ShaderType type, const char *name ) {
			std::string path;

			switch( type ) {
			case VERTEX:
				path = String::Format( "shaders/v_%s.glsl", name );
				break;
			case FRAGMENT:
				path = String::Format( "shaders/f_%s.glsl", name );
				break;
			default:
				throw( XSError( String::Format( "Shader(): Unknown shader type %d", type ).c_str() ) );
			}

			const File f( path.c_str(), FileMode::READ );
			if ( !f.open )
				throw( XSError( String::Format( "Shader(): Could not open file \"%s\"", name ).c_str() ) );

			char *contents = new char[f.length];
				f.Read( (byte *)contents );
				Create( name, contents, shaderTypes[type] );
			delete[] contents;
		}

		Shader::~Shader() {
			glDeleteShader( id );
		}

		//
		// Shader Programs
		//

		// save some typing..
		ShaderProgram::ShaderProgram( const char *vertexShaderName, const char *fragmentShaderName ) {
			Shader *fragmentShader = nullptr;
			Shader *vertexShader = nullptr;

			id = 0;
			uniforms = attributes = NULL;

			id = glCreateProgram();
			if ( !id )
				throw( XSError( "Failed to create shader program" ) );

			if ( vertexShaderName && !vertexShader ) {
				vertexShader = new Shader( Shader::VERTEX, vertexShaderName );
				glAttachShader( id, vertexShader->id );
			}

			if ( fragmentShaderName && !fragmentShader ) {
				fragmentShader = new Shader( Shader::FRAGMENT, fragmentShaderName );
				glAttachShader( id, fragmentShader->id );
			}

			Link();
			Bind();

			if ( vertexShader ) {
				glDetachShader( id, vertexShader->id );
				delete vertexShader;
			}

			if ( fragmentShader ) {
				glDetachShader( id, fragmentShader->id );
				delete fragmentShader;
			}
		}

		ShaderProgram::~ShaderProgram() {
			if ( lastProgramUsed == this )
				lastProgramUsed = NULL;

			glDeleteProgram( id );

			delete uniforms;
			delete attributes;
		}

		// will create if necessary
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
				variable->location = glGetUniformLocation( id, name );
			}

			return variable;
		}

		void ShaderProgram::Link( void ) const {
			int statusCode = 0;

			glLinkProgram( id );
			glGetProgramiv( id, GL_LINK_STATUS, &statusCode );

			if ( statusCode == GL_FALSE ) {
				Console::Print( "Failed to link program %d\n", id );
				OutputProgramInfoLog( id );
			}
		}

		void ShaderProgram::Bind( void ) const {
			if ( lastProgramUsed != this ) {
				glUseProgram( id );
				lastProgramUsed = this;
			}
		}

		void ShaderProgram::SetUniform1( const char *name, int i ) {
			glUniform1i( GetUniformLocation( name )->location, i );
		}

		void ShaderProgram::SetUniform1( const char *name, float f ) {
			glUniform1f( GetUniformLocation( name )->location, f );
		}

		void ShaderProgram::SetUniform2( const char *name, float f1, float f2 ) {
			glUniform2f( GetUniformLocation( name )->location, f1, f2 );
		}

		void ShaderProgram::SetUniform3( const char *name, float f1, float f2, float f3 ) {
			glUniform3f( GetUniformLocation( name )->location, f1, f2, f3 );
		}

		void ShaderProgram::SetUniform4( const char *name, float f1, float f2, float f3, float f4 ) {
			glUniform4f( GetUniformLocation( name )->location, f1, f2, f3, f4 );
		}

	} // namespace Renderer

} // namespace XS
