#include "kepch.h"
#include "DX11Renderer.h"

#include "DaemonEngine/Renderer/Pipeline.h"
#include "DaemonEngine/Renderer/VertexBuffer.h"
#include "DaemonEngine/Renderer/IndexBuffer.h"
#include "DaemonEngine/Renderer/UniformBuffer.h"

#include "DaemonEngine/Core/Application.h"
#include "DaemonEngine/Core/Window.h"

#define BLENDING_TEST

namespace Daemon
{

	void DX11Renderer::Init()
	{
		CreateBlendState();
		CreateDepthStencilState();
	}

	void DX11Renderer::Clear() const
	{
		// Obsolete - Using this might result in strange behaviour, might be removed at a later point
	}

	void DX11Renderer::SetClearColor(const glm::vec4& color) const
	{
		// Obsolete - Using this might result in strange behaviour, might be removed at a later point
	}

	void DX11Renderer::Clear(uint32_t buffer, const glm::vec4& clearColorRGBA) const
	{
		DX11Context& context = DX11Context::Get();
		DX11SwapChain* swapChain = (DX11SwapChain*)Application::Get().GetWindow().GetSwapChain().get();

		// Clear color buffer
		ID3D11RenderTargetView* renderTargetView = swapChain->GetD3DX11RTV();
		context.GetDeviceContext()->ClearRenderTargetView(renderTargetView, &clearColorRGBA.r);

		// Clear depth buffer
		ID3D11DepthStencilView* depthStencilView = swapChain->GetD3DX11DSV();
		context.GetDeviceContext()->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void DX11Renderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
	{
		DX11SwapChain* swapChain = (DX11SwapChain*)Application::Get().GetWindow().GetSwapChain().get();
		swapChain->Resize(width, height);
	}

	void DX11Renderer::BeginFrame()
	{
		DX11Context& context = DX11Context::Get();
		DX11SwapChain* swapChain = (DX11SwapChain*)Application::Get().GetWindow().GetSwapChain().get();
		ID3D11RenderTargetView* renderTargetView = swapChain->GetD3DX11RTV();
		/**** Output Merger *****/
		swapChain->SetRenderTargets(renderTargetView);

		context.GetDeviceContext()->OMSetDepthStencilState(m_DepthStencilState, NULL);
		context.GetDeviceContext()->OMSetBlendState(m_BlendState, NULL, 0xFFFFFFFF);
	}

	void DX11Renderer::EndFrame()
	{
	}

	void DX11Renderer::SubmitFullscreenQuad(Shared<Pipeline> pipeline, Shared<VertexBuffer> vertexBuffer, Shared<IndexBuffer> indexBuffer)
	{
		pipeline->Bind();
		vertexBuffer->Bind();
		indexBuffer->Bind();

		DX11Context& context = DX11Context::Get();
		context.GetDeviceContext()->DrawIndexed(indexBuffer->GetCount(), 0, 0);
	}

	void DX11Renderer::RenderMesh(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<Mesh> mesh, const glm::mat4& transform)
	{
	}

	void DX11Renderer::RenderGeometry(Shared<Pipeline> pipeline, Shared<UniformBuffer> uniformBuffer, Shared<VertexBuffer> vertexBuffer, Shared<IndexBuffer> indexBuffer, uint32_t indexCount)
	{
		if (indexCount == 0)
		{
			indexCount = indexBuffer->GetCount();
		}

		pipeline->Bind();
		vertexBuffer->Bind();
		indexBuffer->Bind();
		uniformBuffer->Bind();

		DX11Context& context = DX11Context::Get();
		context.GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
	}

	void DX11Renderer::CreateBlendState()
	{
		D3D11_BLEND_DESC blendStateDesc;
		ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
#if !defined(BLENDING_TEST)
		blendStateDesc.RenderTarget[0].BlendEnable = false;
		blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
#else
		blendStateDesc.AlphaToCoverageEnable = false;
		blendStateDesc.IndependentBlendEnable = false;

		blendStateDesc.RenderTarget[0].BlendEnable = true;

		blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
#endif
		DX11Context& context = DX11Context::Get();
		HRESULT hr = context.GetDevice()->CreateBlendState(&blendStateDesc, &m_BlendState);
		KE_CORE_ASSERT(!FAILED(hr) && m_BlendState, "Failed to create Blend State");
	}

	void DX11Renderer::CreateDepthStencilState()
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
#if defined(BLENDING_TEST)
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xff;
		depthStencilDesc.StencilWriteMask = 0xff;

		// Stencil operations if pixel is front-facing
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
#else
		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xff;
		depthStencilDesc.StencilWriteMask = 0xff;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
#endif
		DX11Context& context = DX11Context::Get();
		HRESULT hr = context.GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState);
		KE_CORE_ASSERT(!FAILED(hr) && m_BlendState, "Failed to create Blend State");
	}

}