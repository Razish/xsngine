#pragma once

#include "XSCommon/XSVector.h"
#include "XSCommon/XSMatrix.h"

namespace Physics {

	enum class VolumeType : uint32_t {
		OBB,
		Sphere,
	};

	struct OrientedBoundingBoxVolume {

		vector3		mins;
		vector3		maxs;
		matrix4		transform;

	};

	struct SphereVolume {

		real32_t	radius;

	};

	struct Actor {

		VolumeType	type;

		union VolumeData {
			OrientedBoundingBoxVolume obb;
			SphereVolume sphere;
		} volume;

	};

	struct RigidActor : public Actor {

		RigidActor(
			VolumeType type
		);

		// don't allow default instantiation
		RigidActor() = delete;
		RigidActor( const RigidActor& ) = delete;
		RigidActor& operator=( const RigidActor& ) = delete;

	};

} // namespace Physics
