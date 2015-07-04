#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSClient/XSEntityModel.h"

namespace XS {

	namespace ClientGame {

		EntityModel::EntityModel() {
			type = EntityType::Model;
		}

		void EntityModel::Update( real64_t dt ) {
			Entity::Update( dt );
		}

	} // namespace ClientGame

} // namespace XS
