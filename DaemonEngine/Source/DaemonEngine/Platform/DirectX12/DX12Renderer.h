#pragma once
#include "DaemonEngine/Renderer/RendererAPI.h"
#include "DirectX12.h"

#include <vector>

namespace Daemon
{

	class DX12Renderer : public RendererAPI
	{
	public:
		DX12Renderer();
		virtual ~DX12Renderer();

		virtual void Init() override;
		// Obsolete - Using this might result in strange behaviour, might be removed at a later point
		virtual void Clear() const override {}
		// Obsolete - Using this might result in strange behaviour, might be removed at a later point
		virtual void SetClearColor(const glm::vec4& color) const override {}
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const override;
		//virtual void DrawIndexed(const Shared<VertexArray>& vertexArray, uint32_t count) const override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		virtual void SubmitFullscreenQuad(Shared<Pipeline> pipeline, Shared<VertexBuffer> vertexBuffer, Shared<IndexBuffer> indexBuffer) override {}

		virtual void RenderMesh(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<Mesh> mesh, const glm::mat4& transform) override;
		virtual void RenderGeometry(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<VertexBuffer> vertexBuffer, Shared<IndexBuffer> indexBuffer, uint32_t indexCount) override;
		virtual void Clear(uint32_t buffer, const glm::vec4& clearColorRGBA = { 0.2f, 0.2f, 0.2f, 0.2f }) const override;
	private:
		void CreateCommandAllocator();
		void CreateCommandList();
	private:
		ID3D12CommandAllocator* m_CommandAllocator;
		ID3D12GraphicsCommandList* m_CommandList;

		std::vector<ID3D12CommandList*> m_CommandLists;

		// Pipeline objects.
		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;
	};

}
