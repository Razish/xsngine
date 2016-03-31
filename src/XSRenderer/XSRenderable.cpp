#include <limits>
#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSRenderer/XSRenderable.h"

namespace XS {

	namespace Renderer {

		static std::unordered_map<Renderable::Handle, Renderable *> renderables;

		Renderable::Handle Renderable::numRegistered = 0u;
		const Renderable::Handle Renderable::invalidHandle = std::numeric_limits<Renderable::Handle>::max();

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

		Renderable *Renderable::Get( Handle handle ) {
			return renderables[handle];
		}

	} // namespace Renderer

} // namespace XS
