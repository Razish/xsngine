#pragma once

#include <vector>

namespace XS {

	namespace Renderer {

		// forward declarations
		struct VertexAttribute;

		enum class ShaderType {
			Vertex,
			Geometry,
			Fragment,
		};

		class Shader {
		private:
			int			id;
			ShaderType	type;

			// ???
			void Create(
				const char	*path,
				const char	*source,
				ShaderType	 shaderType
			);

		public:
			friend class ShaderProgram;

			// don't allow default instantiation
			Shader() = delete;
			Shader( const Shader& ) = delete;
			Shader& operator=( const Shader& ) = delete;

			Shader(
				ShaderType shaderType,
				const char *name
			);

			~Shader();
		};

		// a ShaderProgram has a list of type ProgramVariable for uniforms
		struct ProgramVariable {
			const char		*name;
			int				 location;

			ProgramVariable()
			: name( nullptr ), location( 0 )
			{
			}
		};


		class ShaderProgram {
		private:
			uint32_t						id;
			std::vector<ProgramVariable>	uniforms;

			// ???
			ProgramVariable &GetUniform(
				const char *name
			);

		public:
			static const ShaderProgram *lastProgramUsed;

			// ???
			static void Init(
				void
			);

			// create a ShaderProgram with a vertex shader and/or a fragment shader and specify default attributes
			//TODO: geometry shader?
			ShaderProgram(
				const char *vertexShaderName,
				const char *fragmentShaderName,
				const VertexAttribute *attributes,
				int numAttributes
			);

			~ShaderProgram();

			// compile and link the ShaderProgram
			void Link(
				void
			) const;

			// use this ShaderProgram for subsequent rendering
			void Bind(
				void
			) const;

			// set the specified uniform variable's value
			//TODO: variadic arguments?
			void SetUniform1(
				const char *name,
				int i
			);
			void SetUniform1(
				const char *name,
				real32_t f
			);
			void SetUniform2(
				const char *name,
				real32_t f1,
				real32_t f2
			);
			void SetUniform3(
				const char *name,
				real32_t f1,
				real32_t f2,
				real32_t f3
			);
			void SetUniform4(
				const char *name,
				real32_t f1,
				real32_t f2,
				real32_t f3,
				real32_t f4
			);

			static uint32_t GetCurrentProgram(
				void
			);
		};

	} // namespace Renderer

} // namespace XS
