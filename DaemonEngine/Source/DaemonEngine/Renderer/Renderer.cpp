#include "kepch.h"
#include "Renderer.h"
#include "Renderer2D.h"

#include "Pipeline.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"

namespace Daemon
{

	struct RendererData
	{
		Shared<ShaderLibrary> ShaderLibrary;
		Shared<Texture2D> WhiteTexture;

		Shared<Pipeline> QuadPipeline;
		Shared<VertexBuffer> QuadVertexBuffer;
		Shared<IndexBuffer> QuadIndexBuffer;
	};

	static Shared<RendererAPI> g_RendererAPI = nullptr;
	static RendererData* s_Data = nullptr;

	void Renderer::Initialize()
	{
		g_RendererAPI = RendererAPI::Create();
		g_RendererAPI->Init();

		s_Data = new RendererData();

		// Create shader library
		s_Data->ShaderLibrary = ShaderLibrary::Create();

		// Load 2D shaders for Renderer2D
		switch (RendererAPI::Current())
		{
			case RendererAPIType::OpenGL:
			{
				s_Data->ShaderLibrary->Load("Renderer2D", "Assets/Shaders/Renderer2D.glsl");
			} break;
			case RendererAPIType::DirectX11:
			case RendererAPIType::DirectX12:
			{
				s_Data->ShaderLibrary->Load("Renderer2D", "Assets/Shaders/Renderer2D.hlsl");
			} break;
		}

		// Create a White Texture
		uint32_t data = 0xFFFFFFFF;
		s_Data->WhiteTexture = Texture2D::Create(1, 1, &data);

		Renderer2D::Initialize();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();

		delete s_Data;
	}

	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Resize(uint32_t width, uint32_t height)
	{
		if (g_RendererAPI)
		{
			g_RendererAPI->SetViewport(0, 0, width, height);
		}
	}

	void Renderer::BeginFrame()
	{
		g_RendererAPI->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		g_RendererAPI->EndFrame();
	}

	void Renderer::RenderMesh(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<Mesh> mesh, const glm::mat4& transform)
	{
		g_RendererAPI->RenderMesh(pipeline, uniformBuffer, mesh, transform);
	}

	void Renderer::RenderGeometry(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<VertexBuffer> vertexBuffer, Shared<IndexBuffer> indexBuffer, uint32_t indexCount)
	{
		g_RendererAPI->RenderGeometry(pipeline, uniformBuffer, vertexBuffer, indexBuffer, indexCount);
	}

	void Renderer::Clear(uint32_t buffer, const glm::vec4& clearColorRGBA)
	{
		g_RendererAPI->Clear(buffer, clearColorRGBA);
	}

	Shared<ShaderLibrary> Renderer::GetShaderLibrary()
	{
		return s_Data->ShaderLibrary;
	}

	Shared<Texture2D> Renderer::GetWhiteTexture()
	{
		return s_Data->WhiteTexture;
	}

}