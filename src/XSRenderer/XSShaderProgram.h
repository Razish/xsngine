#pragma once

namespace XS {

	namespace Renderer {

		enum shaderType_t {
			ST_VERTEX=0,
			ST_FRAGMENT,
			ST_NUM_SHADER_TYPES
		};


		class Shader {
			friend class ShaderProgram;
		private:
			int				id;
			char			name[XS_MAX_FILENAME];
			shaderType_t	type; // fragment/vertex

			Shader(){}
			void Create( const char *path, const char *source, int shaderType );

		public:
			Shader( shaderType_t type, const char *name );
			~Shader();
		};


		class ProgramVariable {
			friend class ShaderProgram;
		private:
			const char		*name;
			int				location;
			ProgramVariable	*next;
		public:
			~ProgramVariable() { delete next; };
		};


		class ShaderProgram {
		private:
			uint32_t		id;
			Shader			*vertexShader, *fragmentShader;
			// TODO: vector
			ProgramVariable	*uniforms, *attributes;

			ProgramVariable *GetUniformLocation( const char *name );

		public:
			static const ShaderProgram *lastProgramUsed;

			static void Init( void );

			ShaderProgram();
			~ShaderProgram();
			ShaderProgram( const char *vertexShaderName, const char *fragmentShaderName );
			void AttachShader( Shader *shader );
			void Link( void );
			void Bind( void );
			void SetUniform1( const char *name, int i );
			void SetUniform1( const char *name, float f );
			void SetUniform2( const char *name, float f1, float f2 );
			void SetUniform3( const char *name, float f1, float f2, float f3 );
			void SetUniform4( const char *name, float f1, float f2, float f3, float f4 );
		};

	} // namespace Renderer

} // namespace XS
