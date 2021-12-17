#pragma once
#include "DaemonEngine/Misc/Camera.h"
#include "Texture2D.h"
#include "SubTexture2D.h"

#include "DaemonEngine/Scene/Components.h"

namespace Daemon
{

	class Renderer2D
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void BeginScene(const Camera& camera);
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void EndScene();

		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Shared<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Shared<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Shared<SubTexture2D>& subtexture, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Shared<SubTexture2D>& subtexture, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void DrawRotatedQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec2& position, float rotation, const glm::vec2& size, const glm::vec4& color);
		static void DrawRotatedQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const Shared<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec2& position, float rotation, const glm::vec2& size, const Shared<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const Shared<SubTexture2D>& subtexture, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void DrawRotatedQuad(const glm::vec2& position, float rotation, const glm::vec2& size, const Shared<SubTexture2D>& subtexture, const glm::vec4& tintColor = glm::vec4(1.0f));

		// base rendering methods
		static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityId = 0);
		static void DrawQuad(const glm::mat4& transform, const Shared<Texture2D>& texture, const glm::vec4& tintColor, int entityId = 0);
		static void DrawQuad(const glm::mat4& transform, const glm::vec2* texCoords, const Shared<SubTexture2D>& subtexture, const glm::vec4& tintColor, int entityId = 0);

		static void DrawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness = 0.02f, int entityId = 0);
		static void DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, int entityId = 0);
		static void DrawCircle(const glm::vec2& position, float radius, const glm::vec4& color, float thickness = 0.03f, int entityId = 0);

		static void DrawSprite(TransformComponent& transform, SpriteRendererComponent& src, int entityId);
	private:
		static void StartBatch();
		static void NextBatch();

		static float BindTexture(const Shared<Texture2D>& texture);
	public:
		struct Statistics
		{
			uint32_t DrawCalls;
			uint32_t QuadCount;

			uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
		};
		static const Statistics& GetStats();
		static void ResetStats();
	};

}
