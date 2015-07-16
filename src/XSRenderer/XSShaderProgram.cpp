#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSCvar.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSVertexAttributes.h"

namespace XS {

	namespace Renderer {

		#define MAX_SHADER_LENGTH (1024*32) // 32kb

		const ShaderProgram *ShaderProgram::tmpBindProgram = nullptr;
		const ShaderProgram *ShaderProgram::lastProgramUsed = nullptr;

		void ShaderProgram::Init( void ) {
			lastProgramUsed = nullptr;
			tmpBindProgram = nullptr;
		}

		static void OutputProgramInfoLog( int program ) {
			int logLength = 0;

			Indent indent( 1 );

			glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logLength );

			if ( logLength > 1 ) {
				std::string logText( logLength - 1, '\0' );

				glGetProgramInfoLog( program, logLength - 1, nullptr, &logText[0] );

				console.Print( PrintLevel::Normal, "%s\n", logText.c_str() );
			}
		}

		static void OutputShaderInfoLog( int shader ) {
			GLint logLength = 0;

			Indent indent( 1 );

			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logLength );

			if ( logLength > 1 ) {
				std::string logText( logLength - 1, '\0' );

				glGetShaderInfoLog( shader, logLength - 1, nullptr, &logText[0] );

				console.Print( PrintLevel::Normal, "%s\n", logText.c_str() );
			}
		}

		//
		// Shaders
		//

		static GLenum GetGLShaderType( ShaderType type ) {
			switch ( type ) {
				case ShaderType::Vertex: {
					return GL_VERTEX_SHADER;
				} break;

				case ShaderType::Geometry: {
					return GL_GEOMETRY_SHADER;
				} break;

				case ShaderType::Fragment: {
					return GL_FRAGMENT_SHADER;
				} break;

				default: {
					return GL_NONE;
				} break;
			}
		}

		void Shader::Create( const char *path, const char *source, ShaderType shaderType ) {
			id = glCreateShader( GetGLShaderType( shaderType ) );
			type = shaderType;

			if ( !id ) {
				throw( XSError( String::Format( "Shader(): Failed to create shader object for shader \"%s\"\n",
					path ).c_str() ) );
			}

			glShaderSource( id, 1, (const GLchar **)&source, nullptr );
			if ( glGetError() == GL_INVALID_OPERATION ) {
				SDL_assert( !"Invalid shader source" );

				OutputShaderInfoLog( id );

				glDeleteShader( id );

				throw( XSError( String::Format( "Shader(): Invalid source code in shader \"%s\"\n", path ).c_str() ) );
			}

			glCompileShader( id );
			int statusCode = 0;
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
			char path[XS_MAX_FILENAME] = {};

			switch ( type ) {
			case ShaderType::Vertex: {
				String::FormatBuffer( path, sizeof(path), "shaders/v_%s.glsl", name );
			} break;

			case ShaderType::Geometry: {
				String::FormatBuffer( path, sizeof(path), "shaders/g_%s.glsl", name );
			} break;

			case ShaderType::Fragment: {
				String::FormatBuffer( path, sizeof(path), "shaders/f_%s.glsl", name );
			} break;
			default: {
				throw( XSError( String::Format( "Shader(): Unknown shader type for \"%s\"", name ).c_str() ) );
			} break;
			}

			const File f( path, FileMode::Read );
			if ( !f.open ) {
				throw( XSError( String::Format( "Shader(): Could not open file \"%s\"", name ).c_str() ) );
			}

			SDL_assert( f.length );

			std::string contents( f.length, '\0' );

			f.Read( reinterpret_cast<uint8_t *>( &contents[0] ) );

			Create( path, contents.c_str(), type );
		}

		Shader::~Shader() {
			glDeleteShader( id );
		}

		//
		// Shader Programs
		//

		// save some typing...
		ShaderProgram::ShaderProgram( const char *vertexShaderName, const char *fragmentShaderName,
			const VertexAttribute *attributes, int numAttributes )
		{
			id = glCreateProgram();
			if ( !id ) {
				throw( XSError( "Failed to create shader program" ) );
			}

			Shader *vertexShader = nullptr;
			if ( vertexShaderName && !vertexShader ) {
				vertexShader = new Shader( ShaderType::Vertex, vertexShaderName );
				glAttachShader( id, vertexShader->id );
			}

			Shader *fragmentShader = nullptr;
			if ( fragmentShaderName && !fragmentShader ) {
				fragmentShader = new Shader( ShaderType::Fragment, fragmentShaderName );
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
				vertexShader = nullptr;
			}

			if ( fragmentShader ) {
				glDetachShader( id, fragmentShader->id );
				delete fragmentShader;
				fragmentShader = nullptr;
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
			auto var = std::find_if(
				std::begin(uniforms),
				std::end(uniforms),
				[name]( const ProgramVariable &uniform ) {
					return !strcmp( uniform.name, name );
				}
			);

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
				console.Print( PrintLevel::Normal, "Failed to link program %d\n", id );
				OutputProgramInfoLog( id );
			}
		}

		void ShaderProgram::Bind( void ) const {
			if ( lastProgramUsed != this ) {
				glUseProgram( id );
				lastProgramUsed = this;
			}
		}

		uint32_t ShaderProgram::GetCurrentProgram( void ) {
			return lastProgramUsed->id;
		}

		void ShaderProgram::CheckBind( void ) {
			#if 0
			if ( !r_fastPath->GetBool() ) {
				if ( lastProgramUsed != this ) {
					console.Print( PrintLevel::Developer, "Temporarily binding shader %02i to set uniform variable\n",
						id
					);
					tmpBindProgram = lastProgramUsed;
					Bind();
				}
			}
			#endif
		};

		void ShaderProgram::CheckUnbind( void ) {
			#if 0
			if ( !r_fastPath->GetBool() ) {
				if ( tmpBindProgram ) {
					tmpBindProgram->Bind();
					tmpBindProgram = nullptr;
				}
			}
			#endif
		};

		//TODO: debug assert that this is the currently bound shader
		void ShaderProgram::SetUniform1( const char *name, int i ) {
			CheckBind();
			glUniform1i( GetUniform( name ).location, i );
			CheckUnbind();
		}

		void ShaderProgram::SetUniform1( const char *name, real32_t f ) {
			CheckBind();
			glUniform1f( GetUniform( name ).location, f );
			CheckUnbind();
		}

		void ShaderProgram::SetUniform1( const char *name, const real32_t *m  ) {
			CheckBind();
			glUniformMatrix4fv( GetUniform( name ).location, 1, GL_FALSE, m );
			CheckUnbind();
		}

		void ShaderProgram::SetUniform2( const char *name, real32_t f1, real32_t f2 ) {
			CheckBind();
			glUniform2f( GetUniform( name ).location, f1, f2 );
			CheckUnbind();
		}

		void ShaderProgram::SetUniform3( const char *name, real32_t f1, real32_t f2, real32_t f3 ) {
			CheckBind();
			glUniform3f( GetUniform( name ).location, f1, f2, f3 );
			CheckUnbind();
		}

		void ShaderProgram::SetUniform4( const char *name, real32_t f1, real32_t f2, real32_t f3, real32_t f4 ) {
			CheckBind();
			glUniform4f( GetUniform( name ).location, f1, f2, f3, f4 );
			CheckUnbind();
		}

	} // namespace Renderer

} // namespace XS
