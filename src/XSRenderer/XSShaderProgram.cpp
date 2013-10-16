#include "XSSystem/XSInclude.h"

#include "GLee/GLee.h"
#include "SDL2/SDL.h"

#include "XSCommon/XSCvar.h"
#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSFile.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderer.h"

namespace XS {

	namespace Renderer {

		#define MAX_SHADER_LENGTH (1024*32) // 32kb

		static Cvar *r_glsl;
		const ShaderProgram *ShaderProgram::lastProgramUsed = NULL;

		static const char *extensionsRequired[] = {
			"GL_ARB_shader_objects",
		};
		static const size_t numExtensionsRequired = ARRAY_LEN( extensionsRequired );

		static const int shaderTypes[ST_NUM_SHADER_TYPES] = {
			GL_VERTEX_SHADER_ARB,
			GL_FRAGMENT_SHADER_ARB
		};

		void ShaderProgram::Init( void ) {
			r_glsl = Cvar::Create( "r_glsl", "1", CVAR_ARCHIVE );

			// let them disable GLSL entirely
			if ( !r_glsl->GetBool() ) {
				Console::Print( "Not using GLSL extension\n" );
				return;
			}

			bool supported = true;
			for ( size_t i=0; i<numExtensionsRequired; i++ ) {
				if ( !SDL_GL_ExtensionSupported( extensionsRequired[i] ) ) {
					supported = false;
					Console::Print( "Warning: Required OpenGL extension '%s' not available\n", extensionsRequired[i] );
				}
			}

			if ( supported )
				Console::Print( "GLSL extension loaded\n" );
			else
				Console::Print( "GLSL extension unavailable\n" );
			r_glsl->Set( supported );

			lastProgramUsed = NULL;
		}

		static void OutputInfoLog( int objectID ) {
			int logLength = 0;
			char *logText = NULL;
			
			Indent indent(1);

			glGetObjectParameterivARB( objectID, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logLength );

			if ( logLength > 1 ) {
				logText = new char[logLength];
				glGetInfoLogARB( objectID, logLength, NULL, logText );

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

			id = glCreateShaderObjectARB( shaderType );
			String::Copy( name, path, sizeof( name ) );
			type = (shaderType == GL_VERTEX_SHADER_ARB) ? ST_VERTEX : ST_FRAGMENT;

			if ( !id ) {
				delete[] shaderCode;
				r_glsl->Set( false );

				throw( String::Format( "Shader(): Failed to create shader object for shader '%s'.\n", path ) );
			}

			glShaderSourceARB( id, 1, (const GLcharARB **)&shaderCode, NULL );
			if ( glGetError() == GL_INVALID_OPERATION ) {
				OutputInfoLog( id );

				glDeleteObjectARB( id );
				delete[] shaderCode;
				r_glsl->Set( false );

				throw( String::Format( "Shader(): Invalid source code in shader '%s'\n", path ) );
			}

			delete[] shaderCode;

			glCompileShaderARB( id );
			glGetObjectParameterivARB( id, GL_OBJECT_COMPILE_STATUS_ARB, &statusCode );

			if ( statusCode == GL_FALSE ) {
				OutputInfoLog( id );

				glDeleteObjectARB( id );
				r_glsl->Set( false );

				throw( String::Format( "Shader(): Failed to compile shader source for shader '%s'\n", path ) );
			}

			OutputInfoLog( id );
		}

		Shader::Shader( shaderType_t type, const char *name ) {
			std::string path;

			switch( type ) {
			case ST_VERTEX:
				path = String::Format( "shaders/v_%s.glsl", name );
				break;
			case ST_FRAGMENT:
				path = String::Format( "shaders/f_%s.glsl", name );
				break;
			default:
				throw( String::Format( "Shader(): Unknown shader type %d", type ) );
			}

			File file( path.c_str(), FM_READ );
			if ( file.length == 0 )
				throw( String::Format( "Shader(): Could not open file '%s'", name ) );

			char *contents = new char[file.length];
				file.Read( (byte *)contents );
				Create( name, contents, shaderTypes[type] );
			delete[] contents;
		}

		Shader::~Shader() {
			glDeleteObjectARB( id );
		}

		//
		// Shader Programs
		//

		ShaderProgram::ShaderProgram() {
			id = 0;
			fragmentShader = vertexShader = NULL;
			uniforms = attributes = NULL;

			if ( !r_glsl->GetBool() )
				return;

			id = glCreateProgramObjectARB();
			if ( !id )
				throw( "Failed to create shader program" );
		}

		// save some typing..
		ShaderProgram::ShaderProgram( const char *vertexShaderName, const char *fragmentShaderName ) {
			id = 0;
			fragmentShader = vertexShader = NULL;
			uniforms = attributes = NULL;

			if ( !r_glsl->GetBool() )
				return;

			id = glCreateProgramObjectARB();
			if ( !id )
				throw( "Failed to create shader program" );

			if ( vertexShaderName && !vertexShader )
				AttachShader( new Shader( ST_VERTEX, vertexShaderName ) );
			if ( fragmentShaderName && !fragmentShader )
				AttachShader( new Shader( ST_FRAGMENT, fragmentShaderName ) );

			Link();
			Bind();
		}

		ShaderProgram::~ShaderProgram() {
			if ( lastProgramUsed == this )
				lastProgramUsed = NULL;

			if ( fragmentShader ) {
				glDetachObjectARB( id, fragmentShader->id );
				delete fragmentShader;
			}

			if ( vertexShader ) {
				glDetachObjectARB( id, vertexShader->id );
				delete vertexShader;
			}

			glDeleteObjectARB( id );

			delete uniforms;
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
			case ST_FRAGMENT:
				if ( fragmentShader != shader ) {
					fragmentShader = shader;
					glAttachObjectARB( id, shader->id );
				}
				break;

			case ST_VERTEX:
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
				Console::Print( "Failed to link program %d\n", id );
				OutputInfoLog( id );
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
