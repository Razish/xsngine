#include "Common/Common.h"
#include "Renderer/Renderer.h"
#include "Renderer/Buffer.h"
#include "Renderer/Material.h"
#include "Renderer/ShaderProgram.h"
#include "Renderer/Texture.h"

namespace Renderer {

	void Material::Bind( void ) const {
		SDL_assert( shaderProgram && "Material::Bind: invalid shader program" );
		shaderProgram->Bind();

		for ( const auto &binding : bufferBindings ) {
			binding.buffer->BindRange( binding.index );
		}

		for ( const auto &binding : samplerBindings ) {
			if ( !binding.uniform.empty() ) {
				// set the sampler binding's texture unit
				//FIXME: optimise/cache the uniform's location
				GLint location = glGetUniformLocation(
					ShaderProgram::GetCurrentProgram(),
					binding.uniform.c_str()
				);
				glUniform1i( location, binding.unit );
			}
			SDL_assert( binding.texture );
			binding.texture->Bind( binding.unit );
		}
	}

} // namespace Renderer
