#pragma once

#include "XSClient/XSEntity.h"

namespace XS {

	namespace ClientGame {

		class EntityModel : public Entity {
		private:

		public:
			EntityModel();

			void Update(
				real64_t dt
			);

		};

	} // namespace ClientGame

} // namespace XS
