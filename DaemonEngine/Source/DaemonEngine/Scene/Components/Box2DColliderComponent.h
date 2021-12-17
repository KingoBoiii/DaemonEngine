#pragma once
#include <glm/glm.hpp>

namespace Daemon
{

	struct Box2DColliderComponent
	{
		glm::vec2 Size = { 0.5f, 0.5f };
		glm::vec2 Offset = { 0.0f, 0.0f };
		float Density = 1.0f;
		float Friction = 1.0f;

		bool UseTransformSize = true;

		void* RuntimeFixture = nullptr;

		Box2DColliderComponent() = default;
		Box2DColliderComponent(const Box2DColliderComponent& other) = default;
		Box2DColliderComponent(const glm::vec2& size) : Size(size) {}
	};

}
