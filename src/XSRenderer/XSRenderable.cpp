#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSRenderer/XSRenderable.h"

namespace XS {

	namespace Renderer {

		static std::unordered_map<uint32_t, Renderable *> renderables;

		uint32_t Renderable::numRegistered = 0u;
		const uint32_t Renderable::invalidHandle = 0xFFFFFFFFu;

		Renderable::Renderable() {
			renderables[handle] = this;
		}

		Renderable::~Renderable() {
			//TODO: refcount..?
			renderables[handle] = nullptr;
		}

		void Renderable::Draw( const RenderInfo &info ) const {
			//TODO: draw view orientation axis?
		}

		void Renderable::Update( real64_t dt ) {
			// ...
		}

		Renderable *Renderable::Get( uint32_t handle ) {
			Renderable *renderable = renderables[handle];
			return renderable;
		}

	} // namespace Renderer

} // namespace XS
