#pragma once
#include "DaemonEngine/Renderer/SwapChain.h"

#include "DirectX11.h"

namespace Daemon
{

	const uint32_t g_FrameCount = 2;

	class DX11SwapChain : public SwapChain
	{
	public:
		DX11SwapChain(const SwapChainSpecification& specification);
		//DX11SwapChain(HWND hwnd, uint32_t width, uint32_t height);
		virtual ~DX11SwapChain();

		virtual void Present() const override;
		virtual void Resize(uint32_t width, uint32_t height) override;

		//void Resize(uint32_t width, uint32_t height);
		void ResizeViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		void SetRenderTargets(ID3D11RenderTargetView* targets);
		void SetRenderTargets(uint32_t count, ID3D11RenderTargetView* const* targets);
		void ResetRenderTargets();

		void Wait();
		void WaitForPreviousFrame();

		virtual uint32_t GetCurrentFrameIndex() const override { return m_FrameIndex; }
		IDXGISwapChain* GetD3DX11SwapChain() const { return m_SwapChain; }
		ID3D11RenderTargetView* GetD3DX11RTV() const { return m_RenderTargetView; }
		ID3D11DepthStencilView* GetD3DX11DSV() const { return m_DepthStencilView; }

		virtual const SwapChainSpecification& GetSpecification() override { return m_Specification; }
		virtual const SwapChainSpecification& GetSpecification() const override { return m_Specification; }
	private:
		void CreateSwapChain(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		void CreateRenderTargetView(ID3D11Device* device);
		void CreateDepthStencilView(ID3D11Device* device);
		void CreateFence(ID3D11Device* device);
	private:
		SwapChainSpecification m_Specification;
		uint32_t m_FrameIndex = 0;

		D3D11_VIEWPORT m_Viewport;
		IDXGISwapChain* m_SwapChain;
		ID3D11RenderTargetView* m_RenderTargetView;
		ID3D11DepthStencilView* m_DepthStencilView;

		uint64_t m_FenceValue = 0;
		ID3D11Fence* m_Fence;
		HANDLE m_FenceEvent;

		friend class DX11Framebuffer;
	};

}
