#pragma once
#include "DaemonEngine/ImGui/ImGuiLayer.h"
#include "DirectX12.h"

namespace Daemon
{

	class DX12ImGuiLayer : public ImGuiLayer
	{
	private:
		struct FrameContext
		{
			ID3D12CommandAllocator* CommandAllocator;
			UINT64                  FenceValue;
		};
	public:
		DX12ImGuiLayer() : ImGuiLayer("DX12ImGui") {}
		virtual ~DX12ImGuiLayer() = default;

		virtual void Begin() override;
		virtual void End() override;
		virtual void BlockEvents(bool block) override { }

		virtual void OnAttach() override;
		virtual void OnDetach() override;
	private:
		void SetupDirectX12ForImGui();
		void CreateRenderTarget(ID3D12Device8* device);
		FrameContext* WaitForNextFrameResources();
	private:
		static int const                    NUM_FRAMES_IN_FLIGHT = 2;
		static int const                    NUM_BACK_BUFFERS = 2;

		FrameContext m_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
		UINT m_frameIndex = 0;

		ID3D12DescriptorHeap* m_pd3dRtvDescHeap;
		ID3D12DescriptorHeap* m_pd3dSrvDescHeap;
		ID3D12CommandQueue* m_pd3dCommandQueue;
		ID3D12GraphicsCommandList* m_pd3dCommandList;
		ID3D12Fence* m_fence;
		HANDLE m_fenceEvent;
		UINT64 m_fenceLastSignaledValue = 0;
		IDXGISwapChain3* m_pSwapChain;
		HANDLE m_hSwapChainWaitableObject;
		ID3D12Resource* m_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
		D3D12_CPU_DESCRIPTOR_HANDLE  m_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};
	};

}