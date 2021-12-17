#pragma once
#include <glm/glm.hpp>
#include "DaemonEngine/Renderer/Texture2D.h"
#include "DaemonEngine/Renderer/SubTexture2D.h"

namespace Daemon
{

	struct QuadData
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Shared<Texture2D> Sprite = nullptr;
		Shared<SubTexture2D> SubSprite = nullptr;
	};

	struct LineData
	{
		glm::vec2 StartPosition = { 0.0f, 0.0f };
		glm::vec2 EndPosition = { 1.0f, 1.0f };
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct CircleData
	{
		float Radius = 1.0f;
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct RectData
	{
		glm::vec2 Size = { 1.0f, 1.0f };
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct SpriteRendererComponent
	{
		enum class PolygonType { Quad, Line, Circle, Rect };
		PolygonType Type = PolygonType::Quad;
		void* Data = nullptr;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent& spriteRenderer) = default;
		SpriteRendererComponent(void* data) : Type(PolygonType::Quad), Data(data) {}
		SpriteRendererComponent(PolygonType type, void* data) : Type(type), Data(data) {}
	};

}
