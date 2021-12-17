#pragma once

namespace Daemon
{

	struct Circle2DColliderComponent
	{
		float Radius = 0.5f;
		float Density = 1.0f;
		float Friction = 1.0f;

		bool UseTransformSize = true;

		void* RuntimeFixture = nullptr;

		Circle2DColliderComponent() = default;
		Circle2DColliderComponent(const Circle2DColliderComponent& other) = default;
		Circle2DColliderComponent(float radius) : Radius(radius) {}
	};

}
