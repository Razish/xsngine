#pragma once

#include <vector>

namespace XS {

	namespace Renderer {

		// forward declarations
		struct VertexAttribute;

		enum class ShaderType {
			UNKNOWN,
			VERTEX,
			GEOMETRY,
			FRAGMENT,
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
				ShaderType type,
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
				float f
			);
			void SetUniform2(
				const char *name,
				float f1,
				float f2
			);
			void SetUniform3(
				const char *name,
				float f1,
				float f2,
				float f3
			);
			void SetUniform4(
				const char *name,
				float f1,
				float f2,
				float f3,
				float f4
			);
		};

	} // namespace Renderer

} // namespace XS
