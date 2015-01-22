#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSError.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSVertexAttributes.h"

namespace XS {

	namespace Renderer {

		#define MAX_SHADER_LENGTH (1024*32) // 32kb

		const ShaderProgram *ShaderProgram::lastProgramUsed = nullptr;

		void ShaderProgram::Init( void ) {
			lastProgramUsed = nullptr;
		}

		static void OutputProgramInfoLog( int program ) {
			int logLength = 0;

			Indent indent( 1 );

			glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logLength );

			if ( logLength > 1 ) {
				std::string logText( logLength - 1, '\0' );

				glGetProgramInfoLog( program, logLength - 1, nullptr, &logText[0] );

				console.Print( "%s\n", logText.c_str() );
			}
		}

		static void OutputShaderInfoLog( int shader ) {
			int logLength = 0;

			Indent indent ( 1 );

			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength );

			if ( logLength > 1 ) {
				std::string logText( logLength - 1, '\0' );

				glGetShaderInfoLog( shader, logLength - 1, nullptr, &logText[0] );

				console.Print( "%s\n", logText.c_str() );
			}
		}

		//
		// Shaders
		//

		static GLenum GetGLShaderType( ShaderType type ) {
			switch ( type ) {
				case ShaderType::VERTEX: {
					return GL_VERTEX_SHADER;
				} break;

				case ShaderType::GEOMETRY: {
					return GL_GEOMETRY_SHADER;
				} break;

				case ShaderType::FRAGMENT: {
					return GL_FRAGMENT_SHADER;
				} break;

				default: {
					return GL_NONE;
				} break;
			}
		}

		void Shader::Create( const char *path, const char *source, ShaderType shaderType ) {
			int statusCode = 0;

			id = glCreateShader( GetGLShaderType( shaderType ) );
			type = shaderType;

			if ( !id ) {
				throw( XSError( String::Format( "Shader(): Failed to create shader object for shader \"%s\"\n",
					path ).c_str() ) );
			}

			glShaderSource( id, 1, (const GLchar **)&source, nullptr );
			if ( glGetError() == GL_INVALID_OPERATION ) {
				OutputShaderInfoLog( id );

				glDeleteShader( id );

				throw( XSError( String::Format( "Shader(): Invalid source code in shader \"%s\"\n", path ).c_str() ) );
			}

			glCompileShader( id );
			glGetShaderiv( id, GL_COMPILE_STATUS, &statusCode );

			if ( statusCode == GL_FALSE ) {
				OutputShaderInfoLog( id );

				glDeleteShader( id );

				throw( XSError( String::Format( "Shader(): Failed to compile shader source for shader \"%s\"\n",
					path ).c_str() ) );
			}

			OutputShaderInfoLog (id);
		}

		Shader::Shader( ShaderType type, const char *name ) {
			char path[XS_MAX_FILENAME] = { '\0' };

			switch ( type ) {
			case ShaderType::VERTEX: {
				String::FormatBuffer( path, sizeof(path), "shaders/v_%s.glsl", name );
			} break;

			case ShaderType::GEOMETRY: {
				String::FormatBuffer( path, sizeof(path), "shaders/g_%s.glsl", name );
			} break;

			case ShaderType::FRAGMENT: {
				String::FormatBuffer( path, sizeof(path), "shaders/f_%s.glsl", name );
			} break;
			default: {
				throw( XSError( String::Format( "Shader(): Unknown shader type for \"%s\"", name ).c_str() ) );
			} break;
			}

			const File f( path, FileMode::READ );
			if ( !f.open ) {
				throw( XSError( String::Format( "Shader(): Could not open file \"%s\"", name ).c_str() ) );
			}

			std::string contents( f.length, '\0' );

			f.Read( reinterpret_cast<uint8_t *>( &contents[0] ) );

			Create( name, contents.c_str(), type );
		}

		Shader::~Shader() {
			glDeleteShader( id );
		}

		//
		// Shader Programs
		//

		// save some typing..
		ShaderProgram::ShaderProgram( const char *vertexShaderName, const char *fragmentShaderName,
			const VertexAttribute *attributes, int numAttributes )
		{
			id = glCreateProgram();
			if ( !id ) {
				throw( XSError( "Failed to create shader program" ) );
			}

			Shader *vertexShader = nullptr;
			if ( vertexShaderName && !vertexShader ) {
				vertexShader = new Shader( ShaderType::VERTEX, vertexShaderName );
				glAttachShader( id, vertexShader->id );
			}

			Shader *fragmentShader = nullptr;
			if ( fragmentShaderName && !fragmentShader ) {
				fragmentShader = new Shader( ShaderType::FRAGMENT, fragmentShaderName );
				glAttachShader( id, fragmentShader->id );
			}

			for ( int i = 0; i < numAttributes; i++ ) {
				glBindAttribLocation( id, attributes[i].location, attributes[i].name );
			}

			Link();
			Bind();

			// Until there are some data files to describe the shader, we'll do it like this for now.
			GLint perFrame = glGetUniformBlockIndex( id, "PerFrame" );
			if ( perFrame >= 0 ) {
				glUniformBlockBinding( id, perFrame, 6 );
			}

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
			if ( lastProgramUsed == this ) {
				lastProgramUsed = nullptr;
			}

			glDeleteProgram( id );
		}

		// will create if necessary
		ProgramVariable &ShaderProgram::GetUniform( const char *name ) {
			auto var = std::find_if( std::begin(uniforms), std::end(uniforms),
				[name](const ProgramVariable& uniform) {
					return strcmp( uniform.name, name ) == 0;
				} );

			if ( var != std::end( uniforms ) ) {
				return *var;
			}

			ProgramVariable uniform;
			uniform.name = name;
			uniform.location = glGetUniformLocation( id, name );

			uniforms.push_back( uniform );

			return uniforms.back();
		}

		void ShaderProgram::Link( void ) const {
			int statusCode = 0;

			glLinkProgram( id );
			glGetProgramiv( id, GL_LINK_STATUS, &statusCode );

			if ( statusCode == GL_FALSE ) {
				console.Print( "Failed to link program %d\n", id );
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
			glUniform1i( GetUniform( name ).location, i );
		}

		void ShaderProgram::SetUniform1( const char *name, float f ) {
			glUniform1f( GetUniform( name ).location, f );
		}

		void ShaderProgram::SetUniform2( const char *name, float f1, float f2 ) {
			glUniform2f( GetUniform( name ).location, f1, f2 );
		}

		void ShaderProgram::SetUniform3( const char *name, float f1, float f2, float f3 ) {
			glUniform3f( GetUniform( name ).location, f1, f2, f3 );
		}

		void ShaderProgram::SetUniform4( const char *name, float f1, float f2, float f3, float f4 ) {
			glUniform4f( GetUniform( name ).location, f1, f2, f3, f4 );
		}

	} // namespace Renderer

} // namespace XS
