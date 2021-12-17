#pragma once
#include "DaemonEngine/Core/PlatformDetection.h"

#include <glm/glm.hpp>
#define USE_PLATFORM_DETECTION 0

namespace Daemon
{

	class Pipeline;
	class VertexBuffer;
	class IndexBuffer;
	class UniformBuffer;
	class Texture2D;
	class Mesh;

	enum class RendererAPIType
	{
		None,
		OpenGL,
		DirectX11,
		DirectX12,
		Vulkan,
	};

	class RendererAPI
	{
	public:
		static Shared<RendererAPI> Create();
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		// Obsolete - Using this might result in strange behaviour, might be removed at a later point
		virtual void Clear() const = 0;
		// Obsolete - Using this might result in strange behaviour, might be removed at a later point
		virtual void SetClearColor(const glm::vec4& color) const = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const = 0;
		//virtual void DrawIndexed(const Shared<VertexArray>& vertexArray, uint32_t count) const = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void SubmitFullscreenQuad(Shared<Pipeline> pipeline, Shared<VertexBuffer> vertexBuffer, Shared<IndexBuffer> indexBuffer) = 0;

		virtual void RenderMesh(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<Mesh> mesh, const glm::mat4& transform) = 0;
		virtual void RenderGeometry(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<VertexBuffer> vertexBuffer, Shared<IndexBuffer> indexBuffer, uint32_t indexCount) = 0;
		virtual void Clear(uint32_t buffer, const glm::vec4& clearColorRGBA) const = 0;

		// Renderer API
		static RendererAPIType Current() { return s_CurrentRendererAPI; }
		static std::string GetRendererName();
		static void SetAPI(RendererAPIType api); 
	private:
#if USE_PLATFORM_DETECTION
	#if defined(KE_PLATFORM_WINDOWS)
		inline static RendererAPIType s_CurrentRendererAPI = RendererAPIType::DirectX11;
	#elif defined(KE_PLATFORM_LINUX)
		inline static RendererAPIType s_CurrentRendererAPI = RendererAPIType::OpenGL;
	#endif
#else
		inline static RendererAPIType s_CurrentRendererAPI = RendererAPIType::DirectX11;
#endif
	};

}
