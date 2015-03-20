#include "XSCommon/XSCommon.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBuffer.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"

namespace XS {

	namespace Renderer {

		void Material::Bind( void ) const {
			SDL_assert( shaderProgram && "Material::Bind: invalid shader program" );
			shaderProgram->Bind();

			for ( const auto &binding : bufferBindings ) {
				binding.buffer->BindRange( binding.index );
			}

			for ( const auto &binding : samplerBindings ) {
				binding.texture->Bind( binding.unit );
			}
		}

	} // namespace Renderer

} // namespace XS
