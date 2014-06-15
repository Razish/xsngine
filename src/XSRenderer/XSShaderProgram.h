#pragma once

namespace XS {

	namespace Renderer {

		struct VertexAttribute;

		enum class ShaderType {
			Vertex,
			Geometry,
			Fragment,
		};

		class Shader {
		public:

			Shader( ShaderType type, const char *name );
			~Shader();

			friend class ShaderProgram;

		private:
			int			id;
			ShaderType	type;

			Shader();
			void Create( const char *path, const char *source, ShaderType shaderType );
		};

		class ProgramVariable {
			friend class ShaderProgram;

			const char		*name;
			int				location;
		};


		class ShaderProgram {
		private:
			uint32_t		id;

			std::vector<ProgramVariable> uniforms;

			ProgramVariable &GetUniform( const char *name );

		public:
			static const ShaderProgram *lastProgramUsed;

			static void Init( void );

			ShaderProgram( const char *vertexShaderName, const char *fragmentShaderName,
				const VertexAttribute *attributes, int numAttributes );
			~ShaderProgram();

			void Link( void ) const;
			void Bind( void ) const;
			void SetUniform1( const char *name, int i );
			void SetUniform1( const char *name, float f );
			void SetUniform2( const char *name, float f1, float f2 );
			void SetUniform3( const char *name, float f1, float f2, float f3 );
			void SetUniform4( const char *name, float f1, float f2, float f3, float f4 );
		};

	} // namespace Renderer

} // namespace XS
