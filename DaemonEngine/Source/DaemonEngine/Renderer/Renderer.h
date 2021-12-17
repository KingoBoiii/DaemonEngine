#pragma once
#include "RendererAPI.h"
#include "Shader.h"
#include "Mesh.h"

namespace Daemon
{
	
	class Texture2D;
	class UniformBuffer;

	class Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void BeginScene();
		static void EndScene();

		static void Resize(uint32_t width, uint32_t height);

		static void BeginFrame();
		static void EndFrame();

		static void RenderMesh(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<Mesh> mesh, const glm::mat4& transform);
		static void RenderGeometry(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<VertexBuffer> vertexBuffer, Shared<IndexBuffer> indexBuffer, uint32_t indexCount = 0);
		static void Clear(uint32_t buffer, const glm::vec4& clearColorRGBA = { 0.2f, 0.2f, 0.2f, 0.2f });

		static Shared<ShaderLibrary> GetShaderLibrary();
		static Shared<Texture2D> GetWhiteTexture();
	};

}
