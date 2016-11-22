#include "XSCommon/XSCommon.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBuffer.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"

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
