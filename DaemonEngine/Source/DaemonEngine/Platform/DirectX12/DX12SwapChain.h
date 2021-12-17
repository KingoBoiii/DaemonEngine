#pragma once
#include "DirectX12.h"

namespace Daemon
{

	class DX12SwapChain
	{
	public:
		DX12SwapChain(HWND handle);
		~DX12SwapChain();

		void Present();
		void WaitForPreviousFrame();

		uint32_t GetCurrentBackBufferIndex() const { return m_SwapChain->GetCurrentBackBufferIndex(); }
		uint32_t GetCurrentFrameIndex() const { return m_FrameIndex; }
		ID3D12Resource* GetD3DX12RTVForFrame(uint32_t frameIndex) const { return m_RenderTargets[frameIndex]; }
		ID3D12Resource* GetD3DX12RTVForCurrentFrame() const { return m_RenderTargets[m_FrameIndex]; }
		ID3D12CommandQueue* GetD3DX12CommandQueue() const { return m_CommandQueue; }
	private:
		void CreateCommandQueue(ID3D12Device8* device);
		void CreateRenderViewTarget(ID3D12Device8* device);
		void CreateFence(ID3D12Device8* device);
	private:
		UINT m_FrameIndex;
		static const UINT FrameCount = 2;

		IDXGISwapChain3* m_SwapChain;
		ID3D12CommandQueue* m_CommandQueue;

		// Render Target View 
		ID3D12DescriptorHeap* m_RTVHeap;
		UINT m_RTVDescriptorSize;
		ID3D12Resource* m_RenderTargets[FrameCount];

		UINT m_FenceValue = 0;
		ID3D12Fence* m_Fence;
		HANDLE m_FenceEvent;

		friend class DX12Pipeline;
		friend class DX12Renderer;
		friend class DX12Context;
	};

}
