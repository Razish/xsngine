#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBuffer.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"

namespace XS {

	namespace Renderer {

		Material::SamplerBinding::SamplerBinding( Texture *texture, const char *uniform, int unit )
		: texture( texture ), unit( unit )
		{
			if ( uniform ) {
				String::Copy( this->uniform, uniform, sizeof(this->uniform) );
			}
			else {
				this->uniform[0] = '\0';
			}
		}

		void Material::Bind( void ) const {
			SDL_assert( shaderProgram && "Material::Bind: invalid shader program" );
			shaderProgram->Bind();

			for ( const auto &binding : bufferBindings ) {
				binding.buffer->BindRange( binding.index );
			}

			for ( const auto &binding : samplerBindings ) {
				if ( !binding.uniform[0] ) {
					// set the sampler binding's texture unit
					//FIXME: optimise/cache the uniform's location
					GLint location = glGetUniformLocation( ShaderProgram::GetCurrentProgram(), binding.uniform );
					glUniform1i( location, binding.unit );
				}
				binding.texture->Bind( binding.unit );
			}
		}

	} // namespace Renderer

} // namespace XS
