#pragma once
#include "DaemonEngine/Physics/2D/Physics2D.h"

namespace Daemon
{

	struct Rigidbody2DComponent
	{
		enum class Type { Static, Dynamic, Kinematic };
		Type BodyType = Type::Static;
		float AngularDamping = 1.0f;
		float LinearDamping = 0.0f;

		void* RuntimeBody = nullptr;	// Runtime storage

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent& rigidbody2D) = default;
		Rigidbody2DComponent(const Type bodyType) : BodyType(bodyType) { }
	};

}
